#include <iostream>
#include <string>
#include <cstring>

#include "mz.h"
#include "mz_os.h"
#include "mz_zip.h"
#include "mz_strm_os.h"

int main() {
    std::string internal_filename;
    std::cout << "Nome do arquivo para Zip: ";
    std::getline(std::cin, internal_filename);

    std::string zip_filename;
    std::cout << "Nome do Zip: ";
    std::getline(std::cin, zip_filename);

    const char* text = "Hello, Marcos, You are awesome\n";
    int32_t text_len = static_cast<int32_t>(strlen(text));

    void* zip = mz_zip_create();
    void* stream = mz_stream_os_create();

    if (!zip || !stream) {
        return 1;
    }

    if (mz_stream_os_open(stream, zip_filename.c_str(), MZ_OPEN_MODE_CREATE | MZ_OPEN_MODE_WRITE) != MZ_OK) {
        return 1;
    }

    if (mz_zip_open(zip, stream, MZ_OPEN_MODE_WRITE) != MZ_OK) {
        return 1;
    }

    std::string filename_copy = internal_filename;

    mz_zip_file file_info = {};
    file_info.version_madeby = MZ_VERSION_MADEBY;
    file_info.filename = filename_copy.c_str();
    file_info.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
    file_info.uncompressed_size = text_len;

    int32_t result = mz_zip_entry_write_open(zip, &file_info, MZ_COMPRESS_LEVEL_DEFAULT, text_len, nullptr);

    result = mz_zip_entry_write(zip, text, text_len);

    mz_zip_entry_close(zip);
    mz_zip_close(zip);
    mz_zip_delete(&zip);
    mz_stream_os_close(stream);
    mz_stream_os_delete(&stream);

    std::cout << "Arquivo ZIP criado!\n";
    return 0;
}
