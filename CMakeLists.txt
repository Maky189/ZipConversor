cmake_minimum_required(VERSION 3.14)
project(ZipConversor)

set(CMAKE_CXX_STANDARD 17)

# Add minizip
add_subdirectory(external/minizip)

# Your executable
add_executable(ZipConversor main.cpp)

# Link minizip to your project
target_link_libraries(ZipConversor minizip)
