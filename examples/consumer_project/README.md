# Example: Using design-patterns-cpp14 from GitHub Packages

This directory contains a complete example of how to use the `design-patterns-cpp14` package
from GitHub Packages in your own project.

## Prerequisites

1. **GitHub Personal Access Token (PAT)**
   - Go to: https://github.com/settings/tokens/new
   - Required scopes: `read:packages`, `repo`
   - Save the token

2. **Conan installed**
   ```bash
   pip install 'conan>=2.0.0,<3.0.0'
   ```

3. **CMake 3.15+**
   ```bash
   cmake --version
   ```

## Setup

### Step 1: Configure Conan Remote (One-time)

```bash
# Add GitHub Packages remote
conan remote add github \
  "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan"

# Login (use your GitHub username and token)
conan remote login github YOUR_GITHUB_USERNAME -p YOUR_GITHUB_TOKEN
```

### Step 2: Install Dependencies

```bash
mkdir build
cd build
conan install .. --remote=github
```

This will:
- Download the `design-patterns-cpp14` package from GitHub Packages
- Generate `conaninfo.txt`, `conan_toolchain.cmake`, and `dedesign-patterns-cpp14Config.cmake`

### Step 3: Build the Project

```bash
# Still in build directory
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Step 4: Run

```bash
./example_app
```

Expected output:
```
Using design-patterns-cpp14 from GitHub Packages!
```

## Files in This Example

- **conanfile.txt** - Conan package configuration
- **CMakeLists.txt** - CMake build configuration
- **main.cpp** - Example C++ code using design-patterns library
- **.github/workflows/build.yml** - CI/CD workflow example

## Using in Your Own Project

Copy these patterns to your project:

### conanfile.txt
```ini
[requires]
design-patterns-cpp14/1.0.24@makiolo/stable

[generators]
CMakeDeps
CMakeToolchain
```

### CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.15)
project(MyApp CXX)

set(CMAKE_CXX_STANDARD 14)

find_package(design-patterns-cpp14 REQUIRED CONFIG)

add_executable(myapp main.cpp)
target_link_libraries(myapp design-patterns-cpp14::design-patterns-cpp14)
```

### C++ Code
```cpp
#include <factory.h>
#include <memoize.h>
#include <iostream>

int main() {
    // Use patterns from design-patterns-cpp14
    std::cout << "Hello from design-patterns-cpp14!" << std::endl;
    return 0;
}
```

## GitHub Actions Integration

For CI/CD, use this in your `.github/workflows/build.yml`:

```yaml
name: Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Setup Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.11'
      
      - name: Install dependencies
        run: |
          pip install cmake 'conan>=2.0.0,<3.0.0'
      
      - name: Configure Conan
        run: |
          conan remote add github \
            "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan" \
            -f --insecure
          conan remote login github github-actions -p ${{ secrets.GITHUB_TOKEN }}
      
      - name: Install Conan dependencies
        run: |
          mkdir build && cd build
          conan install .. --remote=github
      
      - name: Build
        run: |
          cd build
          cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
          cmake --build .
      
      - name: Run
        run: ./build/example_app
```

## Environment Variables

For CI/CD systems, you can use environment variables:

```bash
export CONAN_USERNAME=github-actions
export CONAN_PASSWORD=${{ secrets.GITHUB_TOKEN }}

conan remote login github github-actions --password-stdin << EOF
$CONAN_PASSWORD
EOF
```

## Troubleshooting

### "401 Unauthorized"
- Verify your PAT token is correct
- Check token has `read:packages` scope
- Try logging in again: `conan remote login github USERNAME -p TOKEN`

### "Package not found"
- Check version number is correct
- List available versions: `conan search "design-patterns-cpp14/*" --remote=github`
- Ensure GitHub Packages remote is configured

### "Connection timeout"
- Check internet connection
- Try with `--insecure` flag: `conan install .. --remote=github --insecure`

## More Information

- [GitHub Packages Documentation](../../docs/GITHUB_PACKAGES.md)
- [Project README](../../README.md)
- [Distribution Options](../../docs/DISTRIBUTION.md)

---

**Need help?** Open an issue: https://github.com/makiolo/design-patterns-cpp14/issues
