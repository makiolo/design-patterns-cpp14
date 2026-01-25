#!/bin/bash
# Script to build and run example using Conan

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=== Building example with Conan-managed dependencies ==="

# Create build directory
mkdir -p "$SCRIPT_DIR/build"
cd "$SCRIPT_DIR/build"

# Install Conan dependencies
echo "Installing Conan dependencies..."
conan install .. --build=missing

# Configure CMake
echo "Configuring CMake..."
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building example..."
cmake --build . --config Release

echo "=== Example built successfully ==="
echo "Run with: $SCRIPT_DIR/build/example_factory"
