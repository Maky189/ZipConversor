#include <gtkmm.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <zip.h>

class ZipConversorWindow : public Gtk::Window {
public:
    ZipConversorWindow();

protected:
    void on_button_select_file_clicked();
    void on_button_select_folder_clicked();
    void on_button_convert_clicked();
    void on_button_quit_clicked();

    Gtk::Box m_vbox;
    Gtk::Box m_hbox_buttons;
    Gtk::Box m_hbox_selection;
    Gtk::Label m_label_selected_item;
    Gtk::Button m_button_select_file;
    Gtk::Button m_button_select_folder;
    Gtk::Button m_button_convert;
    Gtk::Button m_button_quit;
    Gtk::ProgressBar m_progress_bar;
    Gtk::TextView m_text_view_log;
    Gtk::ScrolledWindow m_scrolled_window;
    Gtk::ComboBoxText m_format_combo;

    enum class ArchiveFormat {
        ZIP,
        TARGZ,
        SEVENZIP
    };
    ArchiveFormat m_selected_format;

    std::string m_selected_path;
    bool m_is_folder;
    std::string uncodePath(const std::string& path);
    bool convertToZip(const std::string& path, bool is_folder);
    bool addFileToZip(zip_t* zip, const std::filesystem::path& file_path, const std::string& archive_path);
    bool addDirectoryToZip(zip_t* zip, const std::filesystem::path& dir_path, const std::string& base_archive_path = "");
    void log_message(const std::string& message);
    bool convert_to_targz(const std::string& path);
    bool convert_to_7z(const std::string& path);
    std::string get_output_extension() const;
};

ZipConversorWindow::ZipConversorWindow()
    : m_vbox(Gtk::ORIENTATION_VERTICAL, 10),
      m_hbox_buttons(Gtk::ORIENTATION_HORIZONTAL, 5),
      m_hbox_selection(Gtk::ORIENTATION_HORIZONTAL, 5),
      m_label_selected_item("Nenhum arquivo ou pasta selecionado"),
      m_button_select_file("Selecionar Arquivo"),
      m_button_select_folder("Selecionar Pasta"),
      m_button_convert("Converter para ZIP"),
      m_button_quit("Sair"),
      m_is_folder(false),
      m_selected_format(ArchiveFormat::ZIP)
{
    set_title("ZipConversor - Conversor de Arquivos e Pastas para ZIP");
    set_default_size(700, 500);
    set_border_width(10);

    m_button_convert.set_sensitive(false);
    m_progress_bar.set_show_text(true);
    m_progress_bar.set_text("Pronto");

    m_text_view_log.set_editable(false);
    m_scrolled_window.add(m_text_view_log);
    m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_scrolled_window.set_size_request(-1, 200);

    m_vbox.pack_start(m_label_selected_item, Gtk::PACK_SHRINK);
    
    m_hbox_selection.pack_start(m_button_select_file, Gtk::PACK_EXPAND_WIDGET);
    m_hbox_selection.pack_start(m_button_select_folder, Gtk::PACK_EXPAND_WIDGET);
    m_format_combo.append("ZIP");
    m_format_combo.append("TAR.GZ");
    m_format_combo.append("7Z");
    m_format_combo.set_active(0);
    m_hbox_selection.pack_start(m_format_combo, Gtk::PACK_EXPAND_WIDGET);
    m_vbox.pack_start(m_hbox_selection, Gtk::PACK_SHRINK);

    m_hbox_buttons.pack_start(m_button_convert, Gtk::PACK_EXPAND_WIDGET);
    m_hbox_buttons.pack_start(m_button_quit, Gtk::PACK_SHRINK);
    m_vbox.pack_start(m_hbox_buttons, Gtk::PACK_SHRINK);

    m_vbox.pack_start(m_progress_bar, Gtk::PACK_SHRINK);
    m_vbox.pack_start(m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);

    add(m_vbox);

    m_button_select_file.signal_clicked().connect(
        sigc::mem_fun(*this, &ZipConversorWindow::on_button_select_file_clicked));
    m_button_select_folder.signal_clicked().connect(
        sigc::mem_fun(*this, &ZipConversorWindow::on_button_select_folder_clicked));
    m_button_convert.signal_clicked().connect(
        sigc::mem_fun(*this, &ZipConversorWindow::on_button_convert_clicked));
    m_button_quit.signal_clicked().connect(
        sigc::mem_fun(*this, &ZipConversorWindow::on_button_quit_clicked));

    m_format_combo.signal_changed().connect([this]() {
        std::string format = m_format_combo.get_active_text();
        if (format == "ZIP") m_selected_format = ArchiveFormat::ZIP;
        else if (format == "TAR.GZ") m_selected_format = ArchiveFormat::TARGZ;
        else if (format == "7Z") m_selected_format = ArchiveFormat::SEVENZIP;

        std::string button_text = "Converter para " + format;
        m_button_convert.set_label(button_text);
    });

    show_all_children();

    log_message("ZipConversor iniciado. Selecione um arquivo ou pasta para converter para ZIP.");
}

void ZipConversorWindow::on_button_select_file_clicked() {
    Gtk::FileChooserDialog dialog(*this, "Selecione um arquivo para converter para ZIP");
    dialog.set_transient_for(*this);

    dialog.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Abrir", Gtk::RESPONSE_OK);

    auto filter_all = Gtk::FileFilter::create();
    filter_all->set_name("Todos os arquivos");
    filter_all->add_pattern("*");
    dialog.add_filter(filter_all);

    auto filter_text = Gtk::FileFilter::create();
    filter_text->set_name("Arquivos de texto");
    filter_text->add_mime_type("text/plain");
    dialog.add_filter(filter_text);

    int result = dialog.run();

    if (result == Gtk::RESPONSE_OK) {
        m_selected_path = dialog.get_filename();
        m_is_folder = false;
        std::string display_name = std::filesystem::path(m_selected_path).filename().string();
        m_label_selected_item.set_text("Arquivo selecionado: " + display_name);
        m_button_convert.set_sensitive(true);
        log_message("Arquivo selecionado: " + m_selected_path);
    }
}

void ZipConversorWindow::on_button_select_folder_clicked() {
    Gtk::FileChooserDialog dialog(*this, "Selecione uma pasta para converter para ZIP", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    dialog.set_transient_for(*this);

    dialog.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Selecionar", Gtk::RESPONSE_OK);

    int result = dialog.run();

    if (result == Gtk::RESPONSE_OK) {
        m_selected_path = dialog.get_filename();
        m_is_folder = true;
        std::string display_name = std::filesystem::path(m_selected_path).filename().string();
        m_label_selected_item.set_text("Pasta selecionada: " + display_name);
        m_button_convert.set_sensitive(true);
        log_message("Pasta selecionada: " + m_selected_path);
    }
}

void ZipConversorWindow::on_button_convert_clicked() {
    if (m_selected_path.empty()) {
        log_message("Erro: Nenhum arquivo ou pasta selecionado!");
        return;
    }

    m_button_convert.set_sensitive(false);
    m_button_select_file.set_sensitive(false);
    m_button_select_folder.set_sensitive(false);
    m_progress_bar.set_text("Convertendo...");
    m_progress_bar.pulse();

    while (Gtk::Main::events_pending()) {
        Gtk::Main::iteration();
    }

    bool success = false;
    if (m_selected_format == ArchiveFormat::ZIP) {
        success = convertToZip(m_selected_path, m_is_folder);
    } else if (m_selected_format == ArchiveFormat::TARGZ) {
        success = convert_to_targz(m_selected_path);
    } else if (m_selected_format == ArchiveFormat::SEVENZIP) {
        success = convert_to_7z(m_selected_path);
    }

    m_button_convert.set_sensitive(true);
    m_button_select_file.set_sensitive(true);
    m_button_select_folder.set_sensitive(true);

    if (success) {
        m_progress_bar.set_text("Conversão concluída com sucesso!");
        log_message("Conversão concluída com sucesso!");
    } else {
        m_progress_bar.set_text("Erro na conversão!");
        log_message("Erro durante a conversão!");
    }
}

void ZipConversorWindow::on_button_quit_clicked() {
    hide();
}

std::string ZipConversorWindow::uncodePath(const std::string& path) {
    if (path.empty()) return path;

    if (path[0] == '~') {
        const char* home = std::getenv("HOME");
        if (home) {
            return std::string(home) + path.substr(1);
        }
    }

    return path;
}

bool ZipConversorWindow::addFileToZip(zip_t* zip, const std::filesystem::path& file_path, const std::string& archive_path) {
    try {
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            log_message("Erro: Não foi possível abrir o arquivo: " + file_path.string());
            return false;
        }

        auto file_size = std::filesystem::file_size(file_path);
        std::vector<char> file_content(file_size);
        file.read(file_content.data(), file_size);
        file.close();

        if (file.gcount() != static_cast<std::streamsize>(file_size)) {
            log_message("Erro: Não foi possível ler todo o conteúdo do arquivo: " + file_path.string());
            return false;
        }

        zip_source_t* source = zip_source_buffer(zip, file_content.data(), file_content.size(), 0);
        if (!source) {
            log_message("Erro: Não foi possível criar fonte de dados para: " + file_path.string());
            return false;
        }

        zip_int64_t index = zip_file_add(zip, archive_path.c_str(), source, ZIP_FL_OVERWRITE);
        if (index < 0) {
            log_message("Erro: Não foi possível adicionar arquivo ao ZIP: " + archive_path);
            zip_source_free(source);
            return false;
        }

        if (zip_set_file_compression(zip, index, ZIP_CM_DEFLATE, 9) < 0) {
            log_message("Aviso: Não foi possível definir método de compressão para: " + archive_path);
        }

        log_message("Adicionado: " + archive_path + " (" + std::to_string(file_size) + " bytes)");

        while (Gtk::Main::events_pending()) {
            Gtk::Main::iteration();
        }
        
        return true;
    } catch (const std::exception& e) {
        log_message("Erro ao processar arquivo " + file_path.string() + ": " + e.what());
        return false;
    }
}

bool ZipConversorWindow::addDirectoryToZip(zip_t* zip, const std::filesystem::path& dir_path, const std::string& base_archive_path) {
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::filesystem::path relative_path = std::filesystem::relative(entry.path(), dir_path);
                std::string archive_path = base_archive_path.empty() ? relative_path.string() : base_archive_path + "/" + relative_path.string();

                std::replace(archive_path.begin(), archive_path.end(), '\\', '/');
                
                if (!addFileToZip(zip, entry.path(), archive_path)) {
                    return false;
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        log_message("Erro ao processar diretório: " + std::string(e.what()));
        return false;
    }
}

std::string ZipConversorWindow::get_output_extension() const {
    switch (m_selected_format) {
        case ArchiveFormat::ZIP: return ".zip";
        case ArchiveFormat::TARGZ: return ".tar.gz";
        case ArchiveFormat::SEVENZIP: return ".7z";
        default: return ".zip";
    }
}

bool ZipConversorWindow::convertToZip(const std::string& path, bool is_folder) {
    try {
        std::string decoded_path = uncodePath(path);

        if (!std::filesystem::exists(decoded_path)) {
            log_message("Erro: Arquivo ou pasta não encontrado: " + decoded_path);
            return false;
        }

        std::filesystem::path input_path(decoded_path);
        std::string zip_filename;
        
        if (is_folder) {
            if (!std::filesystem::is_directory(input_path)) {
                log_message("Erro: O caminho especificado não é uma pasta: " + decoded_path);
                return false;
            }
            zip_filename = input_path.filename().string() + ".zip";
            log_message("Comprimindo pasta: " + input_path.filename().string());
        } else {
            if (!std::filesystem::is_regular_file(input_path)) {
                log_message("Erro: O caminho especificado não é um arquivo: " + decoded_path);
                return false;
            }
            
            std::string internal_filename = input_path.filename().string();
            
            // Extract filename without extension
            size_t dot_pos = internal_filename.find_last_of('.');
            if (dot_pos != std::string::npos) {
                zip_filename = internal_filename.substr(0, dot_pos);
            } else {
                zip_filename = internal_filename;
            }
            zip_filename += ".zip";
        }

        std::filesystem::path zip_full_path = input_path.parent_path() / zip_filename;

        log_message("ZIP será criado em: " + zip_full_path.string());

        // Create ZIP
        int error;
        zip_t* zip = zip_open(zip_full_path.string().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);

        if (!zip) {
            zip_error_t zip_error;
            zip_error_init_with_code(&zip_error, error);
            log_message("Erro: Não foi possível criar o arquivo ZIP: " + std::string(zip_error_strerror(&zip_error)));
            zip_error_fini(&zip_error);
            return false;
        }

        bool success = false;
        
        if (is_folder) {
            // Compress directory
            success = addDirectoryToZip(zip, input_path);
        } else {
            // Compress single file
            std::string internal_filename = input_path.filename().string();
            success = addFileToZip(zip, input_path, internal_filename);
        }

        if (zip_close(zip) < 0) {
            log_message("Erro: Não foi possível fechar o arquivo ZIP");
            return false;
        }

        if (success) {
            log_message("Arquivo ZIP criado com sucesso: " + zip_full_path.string());

            if (std::filesystem::exists(zip_full_path)) {
                auto zip_size = std::filesystem::file_size(zip_full_path);
                log_message("Tamanho do ZIP: " + std::to_string(zip_size) + " bytes");
            }
        }

        return success;

    } catch (const std::exception& e) {
        log_message("Erro durante a conversão: " + std::string(e.what()));
        return false;
    }
}

bool ZipConversorWindow::convert_to_targz(const std::string& path) {
    try {
        std::filesystem::path input_path(path);
        std::string output_filename = input_path.filename().string() + ".tar.gz";
        std::filesystem::path output_path = input_path.parent_path() / output_filename;

        // Create tar command
        std::string tar_cmd;
        if (std::filesystem::is_directory(input_path)) {
            tar_cmd = "tar -czf '" + output_path.string() + "' -C '" +
                     input_path.parent_path().string() + "' '" +
                     input_path.filename().string() + "'";
        } else {
            tar_cmd = "tar -czf '" + output_path.string() + "' -C '" +
                     input_path.parent_path().string() + "' '" +
                     input_path.filename().string() + "'";
        }

        log_message("Criando arquivo TAR.GZ...");
        int result = system(tar_cmd.c_str());

        if (result == 0) {
            log_message("Arquivo TAR.GZ criado com sucesso: " + output_path.string());
            if (std::filesystem::exists(output_path)) {
                auto size = std::filesystem::file_size(output_path);
                log_message("Tamanho do TAR.GZ: " + std::to_string(size) + " bytes");
            }
            return true;
        } else {
            log_message("Erro ao criar arquivo TAR.GZ");
            return false;
        }
    } catch (const std::exception& e) {
        log_message("Erro durante a criação do TAR.GZ: " + std::string(e.what()));
        return false;
    }
}

bool ZipConversorWindow::convert_to_7z(const std::string& path) {
    try {
        std::filesystem::path input_path(path);
        std::string output_filename = input_path.filename().string() + ".7z";
        std::filesystem::path output_path = input_path.parent_path() / output_filename;

        // Create 7z command
        std::string sevenzip_cmd;
        if (std::filesystem::is_directory(input_path)) {
            sevenzip_cmd = "7z a -t7z '" + output_path.string() + "' '" +
                          input_path.string() + "/*'";
        } else {
            sevenzip_cmd = "7z a -t7z '" + output_path.string() + "' '" +
                          input_path.string() + "'";
        }

        log_message("Criando arquivo 7Z...");
        int result = system(sevenzip_cmd.c_str());

        if (result == 0) {
            log_message("Arquivo 7Z criado com sucesso: " + output_path.string());
            if (std::filesystem::exists(output_path)) {
                auto size = std::filesystem::file_size(output_path);
                log_message("Tamanho do 7Z: " + std::to_string(size) + " bytes");
            }
            return true;
        } else {
            log_message("Erro ao criar arquivo 7Z");
            return false;
        }
    } catch (const std::exception& e) {
        log_message("Erro durante a criação do 7Z: " + std::string(e.what()));
        return false;
    }
}

void ZipConversorWindow::log_message(const std::string& message) {
    auto buffer = m_text_view_log.get_buffer();
    auto iter = buffer->end();
    buffer->insert(iter, message + "\n");

    auto mark = buffer->get_insert();
    m_text_view_log.scroll_to(mark);

    while (Gtk::Main::events_pending()) {
        Gtk::Main::iteration();
    }
}

int gui_main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.zipconversor.application");

    ZipConversorWindow window;

    return app->run(window);
}