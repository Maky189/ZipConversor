cmake_minimum_required(VERSION 3.16)
project(ZipConversor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZIP libzip)

if(LIBZIP_FOUND)
    message(STATUS "Using system libzip")
    set(ZIP_LIBRARIES ${LIBZIP_LIBRARIES})
    set(ZIP_INCLUDE_DIRS ${LIBZIP_INCLUDE_DIRS})
    set(ZIP_LIBRARY_DIRS ${LIBZIP_LIBRARY_DIRS})
else()
    message(STATUS "Using bundled minizip")
    set(MZ_BUILD_TESTS OFF CACHE BOOL "Build minizip tests" FORCE)
    set(MZ_BUILD_UNIT_TESTS OFF CACHE BOOL "Build minizip unit tests" FORCE)
    set(MZ_BUILD_FUZZ_TESTS OFF CACHE BOOL "Build minizip fuzz tests" FORCE)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=incompatible-pointer-types -Wno-incompatible-pointer-types")
    
    add_subdirectory(external/minizip)
    set(ZIP_LIBRARIES minizip)
    set(ZIP_INCLUDE_DIRS external/minizip)
    set(ZIP_LIBRARY_DIRS "")
endif()

include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(${ZIP_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

if(ZIP_LIBRARY_DIRS)
    link_directories(${ZIP_LIBRARY_DIRS})
endif()
add_compile_options(${GTKMM_CFLAGS_OTHER})
add_executable(ZipConversor main.cpp gui.cpp)
target_link_libraries(ZipConversor ${GTKMM_LIBRARIES} ${ZIP_LIBRARIES})
target_compile_options(ZipConversor PRIVATE -Wno-deprecated-declarations)