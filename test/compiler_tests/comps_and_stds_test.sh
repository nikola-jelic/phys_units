#!/bin/bash

# Builds the compile probe against every compiler and C++ standard, renders the
# result as a markdown matrix into README.md, and exits non-zero if any cell
# that is expected to pass did not. That exit code is the point: without it a
# broken toolchain leaves CI green.

COMPILERS=("g++" "clang++")
STANDARDS=("11" "14" "17" "20" "23")

# Cells that must pass. A regression in any of these fails the run. To take a
# cell out of the contract, remove it here and say why.
REQUIRED=(
    "g++_C++11" "g++_C++14" "g++_C++17" "g++_C++20" "g++_C++23"
    "clang++_C++11" "clang++_C++14" "clang++_C++17" "clang++_C++20"
    "clang++_C++23"
)

PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER_TESTS="${PROJECT_ROOT}/test/compiler_tests"
BUILD_ROOT="${COMPILER_TESTS}/build"
LOG_DIR="${COMPILER_TESTS}/logs"

declare -A RESULTS

function is_required()
{
    local key="$1"
    local req
    for req in "${REQUIRED[@]}"; do
        [[ "$req" == "$key" ]] && return 0
    done
    return 1
}

function print_matrix()
{
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

    local COL_WIDTH=6
    for COMPILER in "${COMPILERS[@]}"; do
        printf "| %-8s " "$COMPILER"
        for STD in "${STANDARDS[@]}"; do
            local KEY="${COMPILER}_C++${STD}"
            local SYMBOL="❌"
            [[ "${RESULTS[$KEY]}" == "PASS" ]] && SYMBOL="✅"
            printf "| %-*s " $COL_WIDTH "$SYMBOL"
        done
        echo "|"
    done
}

function export_markdown()
{
    local md_file="${PROJECT_ROOT}/README.md"
    local start_marker="<!-- compiler-matrix-start -->"
    local end_marker="<!-- compiler-matrix-end -->"
    local block_file
    block_file="$(mktemp)"

    {
        echo "$start_marker"
        echo ""
        print_matrix
        echo ""
        echo "$end_marker"
    } > "$block_file"

    awk -v start="$start_marker" -v end="$end_marker" -v blockfile="$block_file" \
        'BEGIN { skip = 0 }
         $0 ~ start {
             while ((getline line < blockfile) > 0) print line
             close(blockfile)
             skip = 1
             next
         }
         $0 ~ end { skip = 0; next }
         !skip { print }' "${md_file}" > "${md_file}.new"

    mv "${md_file}.new" "${md_file}"
    rm -f "$block_file"
    echo "README.md updated with compiler matrix"
}

function run_compilation_matrix()
{
    mkdir -p "${LOG_DIR}"

    for COMPILER in "${COMPILERS[@]}"; do
        if ! command -v "$COMPILER" > /dev/null 2>&1; then
            echo "Skipping ${COMPILER}: not installed"
            for STANDARD in "${STANDARDS[@]}"; do
                RESULTS["${COMPILER}_C++${STANDARD}"]="FAIL"
            done
            continue
        fi

        for STANDARD in "${STANDARDS[@]}"; do
            echo "Compiling with $COMPILER and C++ standard $STANDARD..."

            # A fresh directory per cell. Reusing one made CMake reject the
            # second compiler outright, because a cached CMAKE_CXX_COMPILER
            # cannot be changed in place.
            local BUILD_DIR="${BUILD_ROOT}/${COMPILER}_${STANDARD}"
            rm -rf "$BUILD_DIR"

            local KEY="${COMPILER}_C++${STANDARD}"
            local LOG_FILE="${LOG_DIR}/${KEY}.log"

            if cmake -S "${COMPILER_TESTS}" -B "${BUILD_DIR}" \
                    -DCMAKE_CXX_COMPILER="${COMPILER}" \
                    -DCMAKE_CXX_STANDARD="${STANDARD}" > "$LOG_FILE" 2>&1 &&
               cmake --build "${BUILD_DIR}" --target test_program >> "$LOG_FILE" 2>&1
            then
                RESULTS["$KEY"]="PASS"
            else
                RESULTS["$KEY"]="FAIL"
            fi

            rm -rf "$BUILD_DIR"
        done
    done

    rm -rf "${BUILD_ROOT}"
}

function print_failed_logs()
{
    local any=1
    for COMPILER in "${COMPILERS[@]}"; do
        for STD in "${STANDARDS[@]}"; do
            local KEY="${COMPILER}_C++${STD}"
            if [[ "${RESULTS[$KEY]}" == "FAIL" ]]; then
                if [[ $any -eq 1 ]]; then
                    echo ""
                    echo "=== 🔍 Failed Build Logs ==="
                    any=0
                fi
                echo ""
                echo "----- 💥 Log: $KEY -----"
                cat "${LOG_DIR}/${KEY}.log"
            fi
        done
    done
}

function report_required_failures()
{
    local failures=0
    for COMPILER in "${COMPILERS[@]}"; do
        for STD in "${STANDARDS[@]}"; do
            local KEY="${COMPILER}_C++${STD}"
            if is_required "$KEY" && [[ "${RESULTS[$KEY]}" != "PASS" ]]; then
                echo "Required configuration failed: ${KEY}"
                failures=$((failures + 1))
            fi
        done
    done

    if [[ $failures -gt 0 ]]; then
        echo ""
        echo "${failures} required configuration(s) failed."
        return 1
    fi

    echo ""
    echo "All required configurations passed."
    return 0
}

function main()
{
    cd "$COMPILER_TESTS" || {
        echo "Failed to navigate to compiler_tests folder"
        exit 1
    }

    run_compilation_matrix
    echo ""
    print_matrix
    export_markdown
    print_failed_logs
    report_required_failures
}

main "$@"
