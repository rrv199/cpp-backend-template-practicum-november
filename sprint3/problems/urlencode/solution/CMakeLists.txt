cmake_minimum_required(VERSION 3.11)
project(urlencode VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup(TARGETS)
endif()

add_library(urlencode_lib
    src/urlencode.cpp
)

target_include_directories(urlencode_lib PUBLIC src)

enable_testing()
find_package(Threads REQUIRED)

add_executable(urlencode_tests
    tests/gtest_main.cpp
)

target_link_libraries(urlencode_tests PRIVATE 
    urlencode_lib
    CONAN_PKG::gtest
    Threads::Threads
)

add_test(NAME UrlEncodeTests COMMAND urlencode_tests)

if(WIN32)
    target_link_libraries(urlencode_tests ws2_32)
endif()
