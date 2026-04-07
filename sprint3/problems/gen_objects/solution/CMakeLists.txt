cmake_minimum_required(VERSION 3.11)
project(gen_objects VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Подключение Conan
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup(TARGETS)
endif()

# Основная библиотека
add_library(game_lib
    src/loot_generator.cpp
)

target_include_directories(game_lib PUBLIC src)

# Тесты
enable_testing()

find_package(Threads REQUIRED)

add_executable(game_tests
    tests/loot_generator_tests.cpp
    tests/model_tests.cpp
    src/loot_generator.cpp
    src/loot_generator.h
    src/model.h
)

target_include_directories(game_tests PRIVATE src)
target_link_libraries(game_tests PRIVATE 
    CONAN_PKG::catch2
    Threads::Threads
)

add_test(NAME GameTests COMMAND game_tests)

# Для Windows
if(WIN32)
    target_link_libraries(game_tests ws2_32)
endif()
