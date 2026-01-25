#!/bin/bash
# Script para construir y probar el proyecto con Conan

set -e

echo "=== Design Patterns C++14 - Build with Conan ==="

# Create build directory
mkdir -p build
cd build

# Install dependencies and generate toolchain
echo "Installing Conan dependencies..."
conan install .. --build=missing

# Configure with CMake
echo "Configuring CMake..."
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
cmake --build . --config Release

# Run tests
echo "Running tests..."
ctest --output-on-failure

echo "=== Build completed successfully ==="
