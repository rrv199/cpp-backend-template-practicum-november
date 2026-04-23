cmake_minimum_required(VERSION 3.11)
project(urldecode VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup(TARGETS)
endif()

add_library(urldecode_lib
    src/urldecode.cpp
)

target_include_directories(urldecode_lib PUBLIC src)

enable_testing()

add_executable(urldecode_tests
    tests/boost_test_main.cpp
)

target_link_libraries(urldecode_tests PRIVATE 
    urldecode_lib
    CONAN_PKG::boost
)

add_test(NAME UrlDecodeTests COMMAND urldecode_tests)

if(WIN32)
    target_link_libraries(urldecode_tests ws2_32)
endif()
