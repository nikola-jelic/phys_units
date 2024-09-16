#!/bin/sh

echo "Install library"

# Check if build folder exists
if [ -d "../build" ]
then
    echo "Removing old build folder..."
    rm -rf
else
    echo "Creating build folder..."
    mkdir build
fi

cd build

cmake .. -DCMAKE_INSTALL_PREFIX=/usr

cmake --install .
