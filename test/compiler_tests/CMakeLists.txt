cmake_minimum_required(VERSION 3.15...3.30)

project(phys_units_test_lib_support CXX)

# The compiler and the standard are chosen by the driver script through
# -DCMAKE_CXX_COMPILER and -DCMAKE_CXX_STANDARD. Setting CMAKE_CXX_COMPILER
# from inside this file would be too late to have any effect, since the
# compiler is resolved by project() above.
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -pedantic-errors matters here: without it g++ silently accepts constructs
# from later standards as extensions, which is how a C++17 nested namespace
# definition passed as C++11 for so long.
add_compile_options(-Wall -Wextra -Werror -pedantic-errors)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../inc)

add_executable(test_program test_program.cpp)
