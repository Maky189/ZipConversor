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

int cli_main() {
    std::string path_file;
    std::cout << "Insira o caminho do ficheiro: ";
    std::getline(std::cin, path_file);

    path_file = uncodePath(path_file);

    if (!std::filesystem::exists(path_file)) {
        std::cerr << "Erro: Arquivo não encontrado: " << path_file << std::endl;
        return 1;
    }

    // Check if it's a regular file
    if (!std::filesystem::is_regular_file(path_file)) {
        std::cerr << "Erro: O caminho especificado não é um arquivo: " << path_file << std::endl;
        return 1;
    }

    std::string internal_filename = std::filesystem::path(path_file).filename().string();

    std::string zip_filename;
    for (int i = 0; i < internal_filename.size(); i++) {
        if (internal_filename[i] != '.') {
            zip_filename += internal_filename[i];
        }
        else {
            break;
        }
    }
    zip_filename += ".zip";

    std::filesystem::path source_path(path_file);
    std::filesystem::path zip_full_path = source_path.parent_path() / zip_filename;

    std::cout << "ZIP será criado em: " << zip_full_path << std::endl;

    // Obtain the size of the file
    auto file_size = std::filesystem::file_size(path_file);
    std::cout << "Tamanho do arquivo: " << file_size << " bytes" << std::endl;

    // Put it in memory
    std::ifstream file(path_file, std::ios::binary);
    if (!file) {
        std::cerr << "Erro: Não foi possível abrir o arquivo: " << path_file << std::endl;
        return 1;
    }

    std::vector<char> file_content(file_size);
    file.read(file_content.data(), file_size);
    file.close();

    if (file.gcount() != static_cast<std::streamsize>(file_size)) {
        std::cerr << "Erro: Não foi possível ler todo o conteúdo do arquivo." << std::endl;
        return 1;
    }

    // Criar zip
    int error;
    zip_t* zip = zip_open(zip_full_path.string().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);

    if (!zip) {
        zip_error_t zip_error;
        zip_error_init_with_code(&zip_error, error);
        std::cerr << "Erro: Não foi possível criar o arquivo ZIP: " << zip_error_strerror(&zip_error) << std::endl;
        zip_error_fini(&zip_error);
        return 1;
    }

    zip_source_t* source = zip_source_buffer(zip, file_content.data(), file_content.size(), 0);
    if (!source) {
        std::cerr << "Erro: Não foi possível criar fonte de dados: " << zip_strerror(zip) << std::endl;
        zip_close(zip);
        return 1;
    }

    zip_int64_t index = zip_file_add(zip, internal_filename.c_str(), source, ZIP_FL_OVERWRITE);
    if (index < 0) {
        std::cerr << "Erro: Não foi possível adicionar arquivo ao ZIP: " << zip_strerror(zip) << std::endl;
        zip_source_free(source);
        zip_close(zip);
        return 1;
    }

    // Set compression method
    if (zip_set_file_compression(zip, index, ZIP_CM_DEFLATE, 9) < 0) {
        std::cerr << "Aviso: Não foi possível definir método de compressão: " << zip_strerror(zip) << std::endl;
    }

    // Always the close and free memory
    if (zip_close(zip) < 0) {
        std::cerr << "Erro: Não foi possível fechar o arquivo ZIP: " << zip_strerror(zip) << std::endl;
        return 1;
    }

    std::cout << "Arquivo ZIP criado com sucesso: " << zip_full_path << std::endl;
    std::cout << "Arquivo adicionado: " << internal_filename << " (" << file_size << " bytes)" << std::endl;

    // Verify
    if (std::filesystem::exists(zip_full_path)) {
        auto zip_size = std::filesystem::file_size(zip_full_path);
        std::cout << "Tamanho do ZIP: " << zip_size << " bytes" << std::endl;
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