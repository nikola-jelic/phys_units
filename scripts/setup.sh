#!/bin/sh

echo "Install library"

# Default install path
INSTALL_PATH="/usr"
UNINSTALL=false

# Function to display usage
usage() {
    echo "Usage: $0 [-p install_path] [--path install_path] [-u] [--uninstall]"
    echo "  -p, --path install_path   Specify installation path (default: /usr)"
    echo "  -u, --uninstall           Uninstall the project"
    exit 1
}

# Parse command-line options
TEMP=`getopt -o p:u --long path:,uninstall -- "$@"`
if [ $? != 0 ] ; then usage ; exit 1; fi

eval set -- "$TEMP"

while true ; do
    case "$1" in
        -p|--path)
            INSTALL_PATH=$2 ; shift 2 ;;
        -u|--uninstall)
            UNINSTALL=true ; shift ;;
        --) shift ; break ;;
        *) usage;;
    esac
done

shift $((OPTIND -1))

# Set project root
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# Navigate to project root
cd "$PROJECT_ROOT" || { echo "Failed to navigate to project root"; exit 1; }

# Create build directory if it doesn't exist
BUILD_DIR="${PROJECT_ROOT}/build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || { echo "Failed to navigate to build directory"; exit 1; }

# Run CMake configuration with specified install prefix
cmake .. -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" || { echo "CMake configuration failed"; exit 1; }

# Build the project
cmake --build . || { echo "Build project failed"; exit 1; }

if $UNINSTALL; then
    # Uninstall the project
    INSTALL_MANIFEST="${BUILD_DIR}/install_manifest.txt"
    if [ -f "$INSTALL_MANIFEST" ]; then
        cmake --build . --target uninstall || { echo "Uninstall failled"; exit 1; }
    else
        echo "Error: install_manifest.txt not found in ${BUILD_DIR}"
        exit 1
    fi
else
    # Install the project
    cmake --install . || { echo "Install failed"; exit 1; }
fi

