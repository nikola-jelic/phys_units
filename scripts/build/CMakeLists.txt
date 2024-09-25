cmake_minimum_required(VERSION 3.15...3.30)

project(phys_units_test_lib_support)

# Allow setting the C++ standard from environment variable or default to C++ 11
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optionally specify a compiler from environment variable
if(NOT "${CXX_COMPILER}" STREQUAL "")
    set(CMAKE_CXX_COMPILER {CXX_COMPILER})
endif()

# Add compile options
add_compile_options(-Wall -Werror)

# Add an executable for testing
add_executable(test_program test_program.cpp)
