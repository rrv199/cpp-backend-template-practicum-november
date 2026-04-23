cmake_minimum_required(VERSION 3.11)
project(htmldecode VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup(TARGETS)
endif()

add_library(htmldecode_lib
    src/htmldecode.cpp
)

target_include_directories(htmldecode_lib PUBLIC src)

enable_testing()
find_package(Threads REQUIRED)

add_executable(htmldecode_tests
    tests/catch_test_main.cpp
)

target_link_libraries(htmldecode_tests PRIVATE 
    htmldecode_lib
    CONAN_PKG::catch2
    Threads::Threads
)

add_test(NAME HtmlDecodeTests COMMAND htmldecode_tests)

if(WIN32)
    target_link_libraries(htmldecode_tests ws2_32)
endif()
