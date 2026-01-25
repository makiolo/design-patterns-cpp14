# Using design-patterns-cpp14 with Conan

This guide explains how to use the `design-patterns-cpp14` package with Conan package manager.

## Prerequisites

- Conan 2.0 or higher
- CMake 3.15 or higher
- C++14 compatible compiler

## Installation

### Option 1: Using Local Conan Export

To make this package available locally:

```bash
cd design-patterns-cpp14
conan create . --build=missing
```

This will create the package in your local Conan cache with version `1.0.24`.

### Option 2: From Conan Center or Custom Remote

Once published to a Conan repository, you can use it directly.

## Using in Your Project

### Using conanfile.txt (Simple)

Create a `conanfile.txt` in your project root:

```ini
[requires]
design-patterns-cpp14/1.0.24

[generators]
CMakeDeps
CMakeToolchain
```

Install dependencies:

```bash
mkdir build && cd build
conan install .. --build=missing
```

### Using conanfile.py (Advanced)

Create a `conanfile.py` in your project root:

```python
from conan import ConanFile
from conan.tools.cmake import cmake_layout

class MyProjectConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "design-patterns-cpp14/1.0.24"
    generators = "CMakeDeps", "CMakeToolchain"
    
    def layout(self):
        cmake_layout(self)
```

Then install:

```bash
conan install . --build=missing
```

## CMakeLists.txt Integration

### Modern CMake (3.15+)

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyProject CXX)

set(CMAKE_CXX_STANDARD 14)

# Find the package
find_package(design-patterns-cpp14 REQUIRED CONFIG)

# Create executable
add_executable(my_app main.cpp)
target_link_libraries(my_app design-patterns-cpp14::design-patterns-cpp14)
```

## Building Your Project

```bash
mkdir build
cd build

# Install Conan dependencies
conan install .. --build=missing

# Configure and build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Example Usage

See the `examples/` directory for a complete working example:

```bash
cd examples
./build_example.sh
```

## Using Transitive Dependencies

This library has no external dependencies - it's purely header-only based on C++14 features.

## Publishing to Conan Center

To share your package:

1. Upload to a Conan repository (e.g., Conan Center, JFrog Artifactory)
2. Update the `url` field in `conanfile.py` if needed
3. Users can then install with:

```ini
[requires]
design-patterns-cpp14/1.0.24@<user>/<channel>
```

## Header-Only Library

This is a header-only library, meaning:
- No compiled binaries are distributed
- Compilation happens when the package is used
- Smaller package size
- All source is in the `include/` directory

## More Information

- [Conan Official Documentation](https://docs.conan.io/)
- [Creating Packages](https://docs.conan.io/2/tutorial/creating_packages.html)
- [Header-Only Libraries](https://docs.conan.io/2/tutorials/package_binary_model/header_only.html)
