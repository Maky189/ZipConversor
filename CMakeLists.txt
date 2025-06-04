cmake_minimum_required(VERSION 3.10)
project(ZipConversor)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)

add_subdirectory(external/minizip)

add_executable(ZipConversor main.cpp)

target_link_libraries(ZipConversor
    PRIVATE
    minizip
    ZLIB::ZLIB
)

target_include_directories(ZipConversor PRIVATE
    ${CMAKE_SOURCE_DIR}/external/minizip
    ${CMAKE_BINARY_DIR}/external/minizip
    ${CMAKE_SOURCE_DIR}/external/minizip/compat
)