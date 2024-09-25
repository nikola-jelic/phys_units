#!/bin/sh

echo "Install library"

# Default install path
INSTALL_PATH="/usr"
INSTALL=false
UNINSTALL=false

# Function to display usage
usage() {
    echo "Usage: $0 [path=install_path] [install] [uninstall]"
    echo "  path=install_path   Specify installation path (default: /usr)"
    echo "  install             Install the project"
    echo "  uninstall           Uninstall the project"
    exit 1
}

# Parse key-valude pair options
for ARG in "$@"; do
    case $ARG in
        path=*)
            INSTALL_PATH="${ARG#*=}"
            shift
            ;;
        install)
            INSTALL=true
            shift
            ;;
        uninstall)
            UNINSTALL=true
            shift
            ;;
        *)
            usage
            ;;
    esac
done

# Ensure at least one of INSTALL or UNINSTALL is specified
if ! $INSTALL && ! $UNINSTALL; then
    usage
fi

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
elif $INSTALL; then
    # Install the project
    cmake --install . || { echo "Install failed"; exit 1; }
fi

