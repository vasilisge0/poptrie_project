# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(SAIL LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the sources
add_executable(SAIL
    main.cpp
    Fib.cpp
)

# Include directories
# This tells the compiler where to look for header files
target_include_directories(SAIL PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})