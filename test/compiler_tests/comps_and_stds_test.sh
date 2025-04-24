#!/bin/bash

# Name of the script: compile_test.sh

COMPILERS=("g++" "clang++")
STANDARDS=("11" "14" "17" "20" "23")

PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_TESTS="${PROJECT_ROOT}/test/compiler_tests"
TEMP_BUILD_DIR="${COMPILER_TESTS}/build"
LOG_DIR="${COMPILER_TESTS}/logs"

declare -A RESULTS

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
    local md_file="${PROJECT_ROOT}/README.md"
    local temp_file="${md_file}.tmp"
    local start_marker="<!-- compiler-matrix-start -->"
    local end_marker="<!-- compiler-matrix-end -->"

    {
        echo "$start_marker"
        echo ""
        printf "| %-8s " "Compiler"
        for STD in "${STANDARDS[@]}"; do
            printf "| %-5s " "C++$STD"
        done
        echo "|"

        printf "|%s" "----------"
        for _ in "${STANDARDS[@]}"; do
            printf "|%s" "-------"
        done
        echo "|"

        COL_WIDTH=6
        for COMPILER in "${COMPILERS[@]}"; do
            printf "| %-8s " "$COMPILER"
            for STD in "${STANDARDS[@]}"; do
                local KEY="${COMPILER}_C++${STD}"
                local RESULT="${RESULTS[$KEY]}"
                local SYMBOL="❌"
                [[ "$RESULT" == "PASS" ]] && SYMBOL="✅"
                printf "| %-*s " $COL_WIDTH "$SYMBOL"
            done
            echo "|"
        done
        echo ""
        echo "$end_marker"
    } > "$temp_file"

    # Replace the block in README.md
    awk -v start="$start_marker" -v end="$end_marker" \
        -v new="$(cat < "$temp_file" | sed 's/[/]/\\\//g')" \
        'BEGIN{found=0}
         {
           if ($0 ~ start) {print new; found=1; skip=1; next}
           if ($0 ~ end) {skip=0; next}
           if (!skip) print
         }' "${md_file}" > "${md_file}.new"

    mv "${md_file}.new" "${md_file}"
    rm -f "${temp_file}"
    echo "README.md updated with compiler matrix"
}

function run_compilation_matrix()
{
    mkdir -p "${TEMP_BUILD_DIR}" "${LOG_DIR}"

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
            LOG_FILE="${LOG_DIR}/${KEY}.log"
            echo "$BUILD_OUTPUT" > "$LOG_FILE"

            if [ $BUILD_EXIT_CODE -ne 0 ]; then
                RESULTS["$KEY"]="FAIL"
            else
                RESULTS["$KEY"]="PASS"
            fi

            # Clean up build artifacts and leave temporary build folder
            rm -f test_program
            cd "$COMPILER_TESTS" || exit 1
        done
    done

    # Clean up the build directory
    rm -rf "${TEMP_BUILD_DIR}"
}

print_failed_logs()
{
    echo ""
    echo "=== 🔍 Failed Build Logs ==="

    for COMPILER in "${COMPILERS[@]}"; do
        for STD in "${STANDARDS[@]}"; do
            KEY="${COMPILER}_C++${STD}"
            RESULT="${RESULTS[$KEY]}"

            if [[ "$RESULT" == "FAIL" ]]; then
                LOG_FILE="${LOG_DIR}/${KEY}.log"
                echo ""
                echo "----- 💥 Log: $KEY -----"
                cat "$LOG_FILE"
            fi
        done
    done
}

function main()
{
    cd "$PROJECT_ROOT" || {
        echo "Failed to navigate to project root"
        exit 1
    }

    cd "$COMPILER_TESTS" || {
        echo "Failed to navigate to compiler_tests folder"
        exit 1
    }

    run_compilation_matrix
    print_matrix_header
    print_results
    export_markdown
    print_failed_logs
}

main "$@"
