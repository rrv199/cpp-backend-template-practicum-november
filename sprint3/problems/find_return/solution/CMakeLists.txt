cmake_minimum_required(VERSION 3.11)
project(find_return VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(game_lib
    src/game_types.cpp
    src/collision_handler.cpp
)

target_include_directories(game_lib PUBLIC src)

enable_testing()
find_package(Threads REQUIRED)

add_executable(game_tests
    tests/test.cpp
)

target_link_libraries(game_tests PRIVATE 
    game_lib
    Threads::Threads
)

add_test(NAME GameTests COMMAND game_tests)
