cmake_minimum_required(VERSION 3.11)
project(game_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup(TARGETS)
endif()

find_package(Threads REQUIRED)

add_executable(game_server
    src/database.cpp
    src/dog_manager.cpp
    src/records_handler.cpp
    src/main.cpp
)

target_include_directories(game_server PRIVATE src)
target_link_libraries(game_server PRIVATE 
    CONAN_PKG::libpqxx
    CONAN_PKG::boost
    CONAN_PKG::nlohmann_json
    Threads::Threads
)
