cmake_minimum_required(VERSION 3.11)
project(book_manager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup(TARGETS)
endif()

find_package(Threads REQUIRED)

add_executable(book_manager
    src/main.cpp
)

target_include_directories(book_manager PRIVATE src)
target_link_libraries(book_manager PRIVATE 
    CONAN_PKG::libpqxx
    CONAN_PKG::boost
    CONAN_PKG::nlohmann_json
    Threads::Threads
)
