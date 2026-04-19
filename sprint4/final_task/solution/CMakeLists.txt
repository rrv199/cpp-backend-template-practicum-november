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

# Основная библиотека
add_library(game_lib
    src/database.cpp
    src/dog_manager.cpp
    src/records_handler.cpp
)

target_include_directories(game_lib PRIVATE src)
target_link_libraries(game_lib PRIVATE 
    CONAN_PKG::libpqxx
    CONAN_PKG::boost
    CONAN_PKG::nlohmann_json
    Threads::Threads
)

# Основная программа
add_executable(game_server
    src/main.cpp
)

target_link_libraries(game_server PRIVATE game_lib)

# Тесты (если Catch2 установлен)
if(TARGET CONAN_PKG::catch2)
    enable_testing()
    add_executable(game_tests
        tests/test_dog_manager.cpp
    )
    target_include_directories(game_tests PRIVATE src)
    target_link_libraries(game_tests PRIVATE 
        game_lib
        CONAN_PKG::catch2
        Threads::Threads
    )
    add_test(NAME GameTests COMMAND game_tests)
endif()
