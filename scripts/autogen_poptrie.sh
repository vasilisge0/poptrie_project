#!/bin/bash

# Exit on error
set -e

echo "Creating files required for building..."
touch NEWS AUTHORS ChangeLog

echo "Running libtoolize..."
libtoolize

echo "Running aclocal..."
aclocal

echo "Running autoupdate..."
autoupdate

echo "Running autoconf..."
autoconf

echo "Running automake with --add-missing..."
automake --add-missing

# echo "Creating build directory..."
# mkdir -p build
# cd build

echo "Configuring the build..."
../configure

echo "Building the project..."
make

echo "Build complete!"
