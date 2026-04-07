cmake_minimum_required(VERSION 3.11)
project(static_lib VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Подключение Conan
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup(TARGETS)
endif()

# ============================================
# СТАТИЧЕСКАЯ БИБЛИОТЕКА С ИГРОВОЙ МОДЕЛЬЮ
# ============================================
add_library(game_model STATIC
    src/loot_generator.cpp
    src/model.cpp
)

# Подключаем заголовочные файлы
target_include_directories(game_model PUBLIC src)

# ============================================
# ОСНОВНАЯ ПРОГРАММА
# ============================================
add_executable(game_server
    src/main.cpp
)

target_link_libraries(game_server PRIVATE game_model)

# ============================================
# ТЕСТЫ
# ============================================
enable_testing()

find_package(Threads REQUIRED)

add_executable(game_tests
    tests/simple_tests.cpp
)

target_include_directories(game_tests PRIVATE src)
target_link_libraries(game_tests PRIVATE 
    game_model
    CONAN_PKG::catch2
    Threads::Threads
)

add_test(NAME GameTests COMMAND game_tests)

# Для Windows
if(WIN32)
    target_link_libraries(game_tests ws2_32)
endif()
