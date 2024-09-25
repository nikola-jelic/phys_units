#!/bin/bash

# Name of the script: compile_test.sh

COMPILERS=("g++" "clang++")
STANDARDS=("11" "14" "17" "20")
FILE="test_program.cpp"

# Set project root
PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

# Navigate to project root
cd "$PROJECT_ROOT" || { echo "Failed to navigate to project root"; exit 1; }

# Set build directory in scripts and navigate to it
TEST_BUILD="${PROJECT_ROOT}/scripts/build"
cd "$TEST_BUILD" || { echo "Failed to navigate to test build folder"; exit 1; }

# Create build directory if it doesn't exist
TEMP_BUILD_DIR="${PROJECT_ROOT}/scripts/build/build"
mkdir -p "$TEMP_BUILD_DIR"
#cd "$TEMP_BUILD_DIR" || { echo "Failed to navigate to build directory"; exit 1; }

# Loop through compilers and standards
for COMPILER in "${COMPILERS[@]}"
do
    for STANDARD in "${STANDARDS[@]}"
    do
        echo "Compiling with $COMPILER and C++ standard $STANDARD..."

        # Set environment variable
        export CXX_COMPILER=$COMPILER
        export CXX_STANDARD=$STANDARD

        # Configure and build project
        cd "$TEMP_BUILD_DIR" || { echo "Failed to navigate to build directory"; exit 1; }
        cmake -D CMAKE_CXX_STANDARD=${CXX_STANDARD} -D CMAKE_CXX_COMPILER=${CXX_COMPILER} ..
        cmake --build . --target test_program

        if [ $? -ne 0 ]; then
            echo "Compilation failed with $COMPILER and C++$STANDARD standard"
            cd ..
            continue
        else
            echo "Compilation succeeded with $COMPILER and C++$STANDARD standard"
        fi

        # Clean up build artifacts and leave temporary build folder
        rm -f test_program
        cd ..
    done
done

# Clean up the build directory
rm -rf build
