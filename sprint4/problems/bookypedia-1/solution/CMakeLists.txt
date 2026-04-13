cmake_minimum_required(VERSION 3.11)
project(bookypedia VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup(TARGETS)
endif()

find_package(Threads REQUIRED)

add_executable(bookypedia
    src/main.cpp
    src/infrastructure/database_manager.cpp
    src/infrastructure/repositories.cpp
    src/application/use_cases.cpp
)

target_include_directories(bookypedia PRIVATE 
    src
    include
)

target_link_libraries(bookypedia PRIVATE 
    CONAN_PKG::libpqxx
    CONAN_PKG::boost
    Threads::Threads
)
