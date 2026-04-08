cmake_minimum_required(VERSION 3.11)
project(game_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Boost REQUIRED COMPONENTS system log log_setup)

add_executable(game_server
    src/main.cpp
    src/json_loader.cpp
    src/request_handler.cpp
    src/model.cpp
)

target_include_directories(game_server PRIVATE src)

target_link_libraries(game_server PRIVATE 
    CONAN_PKG::boost
    ${Boost_LIBRARIES}
    Threads::Threads
    boost_log_setup
    boost_log
)

if(WIN32)
    target_link_libraries(game_server ws2_32 wsock32)
endif()
