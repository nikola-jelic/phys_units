#!/bin/bash

# Name of the script: compile_test.sh

COMPILERS=("g++" "clang++")
STANDARDS=("11" "14" "17" "20" "23")

# Set project root
PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

function print_matrix_header()
{
    # Print column headers
    printf "| %-8s " "Compiler"
    for STD in "${STANDARDS[@]}"; do
        printf "| %-5s " "C++$STD"
    done
    echo "|"

    # Print separator line
    printf "|%s" "----------" # For "Compiler"
    for STD in "${STANDARDS[@]}"; do
        printf "|%s" "-------"
    done
    echo "|"
}

function print_results()
{
    COL_WIDTH=6
    for COMPILER in "${COMPILERS[@]}"; do
        printf "| %-8s " "$COMPILER"
        for STD in "${STANDARDS[@]}"; do
            KEY="${COMPILER}_C++${STD}"
            RESULT="${RESULTS[$KEY]}"
            if [[ "${RESULT}" == "PASS" ]]; then
                SYMBOL="✅"
            else
                SYMBOL="❌"
            fi
            printf "| %-*s " ${COL_WIDTH} "${SYMBOL}"
        done
        echo "|"
    done
}

function export_markdown()
{
    local md_file="${COMPILER_TESTS}/compile_results.md"
    : > "$md_file" # Clear previous content

    # Header row
    printf "| %-8s " "Compiler" >> "$md_file"
    for STD in "${STANDARDS[@]}"; do
        printf "| %-5s " "C++$STD" >> "$md_file"
    done
    echo "|" >> "$md_file"

    # Separator row
    printf "|%s" "----------" >> "$md_file"
    for STD in "${STANDARDS[@]}"; do
        printf "|%s" "-------" >> "$md_file"
    done
    echo "|" >> "$md_file"

    # Data rows with symbols centered
    COL_WIDTH=6 # Define column width for result symbols
    for COMPILER in "${COMPILERS[@]}"; do
        printf "| %-8s " "$COMPILER" >> "$md_file"
        for STD in "${STANDARDS[@]}"; do
            KEY="${COMPILER}_C++${STD}"
            RESULT="${RESULTS[$KEY]}"
            if [[ "$RESULT" == "PASS" ]]; then
                SYMBOL="✅"
            else
                SYMBOL="❌"
            fi
            printf "| %-*s " $COL_WIDTH "$SYMBOL" >> "$md_file"
        done
        echo "|" >> "$md_file"
    done

    echo "Markdown table exported to: $md_file"
}

# Navigate to project root
cd "$PROJECT_ROOT" || {
    echo "Failed to navigate to project root"
    exit 1
}

# Set build directory in scripts and navigate to it
COMPILER_TESTS="${PROJECT_ROOT}/test/compiler_tests"
cd "$COMPILER_TESTS" || {
    echo "Failed to navigate to compiler_tests folder"
    exit 1
}

# Create build directory if it doesn't exist
TEMP_BUILD_DIR="${COMPILER_TESTS}/build"
mkdir -p "$TEMP_BUILD_DIR"

declare -A RESULTS

# Loop through compilers and standards
for COMPILER in "${COMPILERS[@]}"; do
    for STANDARD in "${STANDARDS[@]}"; do
        echo "Compiling with $COMPILER and C++ standard $STANDARD..."

        # Set environment variable
        export CXX_COMPILER=$COMPILER
        export CXX_STANDARD=$STANDARD

        # Configure and build project
        cd "$TEMP_BUILD_DIR" || {
            echo "Failed to navigate to build directory"
            exit 1
        }
        cmake -D CMAKE_CXX_STANDARD="${CXX_STANDARD}" -D CMAKE_CXX_COMPILER="${CXX_COMPILER}" ..

        BUILD_OUTPUT=$(cmake --build . --target test_program 2>&1)
        BUILD_EXIT_CODE=$?

        KEY="${COMPILER}_C++${STANDARD}"
        if [ $BUILD_EXIT_CODE -ne 0 ]; then
            # echo "Compilation failed with $COMPILER and C++$STANDARD standard"
            # echo "${BUILD_OUTPUT}"
            # cd ..
            # continue
            RESULTS["$KEY"]="FAIL"
        else
            # echo "Compilation succeeded with $COMPILER and C++$STANDARD standard"
            # echo "${BUILD_OUTPUT}"
            RESULTS["$KEY"]="PASS"
        fi

        # Clean up build artifacts and leave temporary build folder
        rm -f test_program
        cd ..
    done
done

# Clean up the build directory
rm -rf "${TEMP_BUILD_DIR}"

print_matrix_header
print_results
export_markdown

exit 0
