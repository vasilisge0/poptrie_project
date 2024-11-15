#!/bin/bash

echo "Building all projects"

echo "Building Poptrie"
cd modified_poptrie
../scripts/autogen_poptrie.sh
cd ..

echo "Building Radix Tree"
cd modified_radix_tree
../scripts/autogen_radix_tree.sh
cd ..

echo "Building Tree Bitmap"
cd modified_tree_bitmap
make all
cd ..

echo "Building SAIL"
cd modified_sail
mkdir -p build
cd build
cmake ..
make