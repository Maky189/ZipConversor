#include <algorithm>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <zip.h>

int gui_main(int argc, char* argv[]);

std::string uncodePath(const std::string& path) {
    if (path.empty()) return path;

    if (path[0] == '~') {
        const char* home = std::getenv("HOME");
        if (home) {
            return std::string(home) + path.substr(1);
        }
    }

    return path;
}

bool addFileToZip(zip_t* zip, const std::filesystem::path& file_path, const std::string& archive_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        std::cerr << "Erro: Não foi possível abrir o arquivo: " << file_path << std::endl;
        return false;
    }

    auto file_size = std::filesystem::file_size(file_path);
    std::vector<char> file_content(file_size);
    file.read(file_content.data(), file_size);
    file.close();

    if (file.gcount() != static_cast<std::streamsize>(file_size)) {
        std::cerr << "Erro: Não foi possível ler todo o conteúdo do arquivo: " << file_path << std::endl;
        return false;
    }

    zip_source_t* source = zip_source_buffer(zip, file_content.data(), file_content.size(), 0);
    if (!source) {
        std::cerr << "Erro: Não foi possível criar fonte de dados para: " << file_path << std::endl;
        return false;
    }

    zip_int64_t index = zip_file_add(zip, archive_path.c_str(), source, ZIP_FL_OVERWRITE);
    if (index < 0) {
        std::cerr << "Erro: Não foi possível adicionar arquivo ao ZIP: " << archive_path << std::endl;
        zip_source_free(source);
        return false;
    }

    // Set compression method
    if (zip_set_file_compression(zip, index, ZIP_CM_DEFLATE, 9) < 0) {
        std::cerr << "Aviso: Não foi possível definir método de compressão para: " << archive_path << std::endl;
    }

    std::cout << "Adicionado: " << archive_path << " (" << file_size << " bytes)" << std::endl;
    return true;
}

bool addDirectoryToZip(zip_t* zip, const std::filesystem::path& dir_path, const std::string& base_archive_path = "") {
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::filesystem::path relative_path = std::filesystem::relative(entry.path(), dir_path);
                std::string archive_path = base_archive_path.empty() ? relative_path.string() : base_archive_path + "/" + relative_path.string();
                
                // Convert backslashes to forward slashes for ZIP compatibility
                std::replace(archive_path.begin(), archive_path.end(), '\\', '/');
                
                if (!addFileToZip(zip, entry.path(), archive_path)) {
                    return false;
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Erro ao processar diretório: " << e.what() << std::endl;
        return false;
    }
}

int cli_main() {
    std::string path_input;
    std::cout << "Insira o caminho do arquivo ou pasta: ";
    std::getline(std::cin, path_input);

    path_input = uncodePath(path_input);

    if (!std::filesystem::exists(path_input)) {
        std::cerr << "Erro: Arquivo ou pasta não encontrado: " << path_input << std::endl;
        return 1;
    }

    std::filesystem::path input_path(path_input);
    std::string zip_filename;
    
    if (std::filesystem::is_regular_file(input_path)) {
        // Handle single file
        std::string internal_filename = input_path.filename().string();
        
        for (int i = 0; i < internal_filename.size(); i++) {
            if (internal_filename[i] != '.') {
                zip_filename += internal_filename[i];
            }
            else {
                break;
            }
        }
        zip_filename += ".zip";
    } else if (std::filesystem::is_directory(input_path)) {
        // Handle directory
        zip_filename = input_path.filename().string() + ".zip";
    } else {
        std::cerr << "Erro: O caminho especificado não é um arquivo nem uma pasta: " << path_input << std::endl;
        return 1;
    }

    std::filesystem::path zip_full_path = input_path.parent_path() / zip_filename;

    std::cout << "ZIP será criado em: " << zip_full_path << std::endl;

    // Create ZIP
    int error;
    zip_t* zip = zip_open(zip_full_path.string().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);

    if (!zip) {
        zip_error_t zip_error;
        zip_error_init_with_code(&zip_error, error);
        std::cerr << "Erro: Não foi possível criar o arquivo ZIP: " << zip_error_strerror(&zip_error) << std::endl;
        zip_error_fini(&zip_error);
        return 1;
    }

    bool success = false;
    
    if (std::filesystem::is_regular_file(input_path)) {
        // Compress single file
        std::string internal_filename = input_path.filename().string();
        success = addFileToZip(zip, input_path, internal_filename);
    } else if (std::filesystem::is_directory(input_path)) {
        // Compress directory
        std::cout << "Comprimindo pasta: " << input_path.filename() << std::endl;
        success = addDirectoryToZip(zip, input_path);
    }

    // Close ZIP
    if (zip_close(zip) < 0) {
        std::cerr << "Erro: Não foi possível fechar o arquivo ZIP" << std::endl;
        return 1;
    }

    if (success) {
        std::cout << "Arquivo ZIP criado com sucesso: " << zip_full_path << std::endl;
        
        // Verify
        if (std::filesystem::exists(zip_full_path)) {
            auto zip_size = std::filesystem::file_size(zip_full_path);
            std::cout << "Tamanho do ZIP: " << zip_size << " bytes" << std::endl;
        }
    } else {
        std::cerr << "Erro durante a criação do ZIP" << std::endl;
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    // CLI mode
    if (argc > 1 && std::string(argv[1]) == "--cli") {
        return cli_main();
    }
    
    // Default to GUI mode
    return gui_main(argc, argv);
}