cmake_minimum_required(VERSION 3.11)
project(gather_tests VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup(TARGETS)
endif()

enable_testing()
find_package(Threads REQUIRED)

add_library(collision_detector STATIC
    src/collision-detector.cpp
)

target_include_directories(collision_detector PUBLIC src)

add_executable(gather_tests
    tests/collision-detector-tests.cpp
)

target_link_libraries(gather_tests PRIVATE 
    collision_detector
    CONAN_PKG::catch2
    Threads::Threads
)

add_test(NAME GatherTests COMMAND gather_tests)

if(WIN32)
    target_link_libraries(gather_tests ws2_32)
endif()
