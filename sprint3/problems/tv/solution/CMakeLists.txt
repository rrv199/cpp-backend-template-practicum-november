cmake_minimum_required(VERSION 3.11)
project(tv VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup(TARGETS)
endif()

add_library(tv_lib
    src/tv.cpp
)

target_include_directories(tv_lib PUBLIC src)

enable_testing()
find_package(Threads REQUIRED)

add_executable(tv_tests
    tests/google_tv_tests.cpp
)

target_link_libraries(tv_tests PRIVATE 
    tv_lib
    CONAN_PKG::gtest
    Threads::Threads
)

add_test(NAME TVTests COMMAND tv_tests)

if(WIN32)
    target_link_libraries(tv_tests ws2_32)
endif()
