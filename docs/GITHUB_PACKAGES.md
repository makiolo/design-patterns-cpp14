# Using design-patterns-cpp14 from GitHub Packages

GitHub Packages hosts the compiled Conan package for easy access without cloning the repository.

## Quick Setup (One-Time)

### 1. Create a Personal Access Token (PAT)

Go to: https://github.com/settings/tokens/new

Required scopes:
- ✅ `read:packages` (read packages)
- ✅ `write:packages` (upload/deploy packages)
- ✅ `repo` (for private repos)

Copy your token - you'll use it below.

### 2. Configure Conan Remote (Local Machine)

```bash
# Add GitHub Packages as a Conan remote
conan remote add github \
  "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan"

# Login with your GitHub credentials
conan remote login github YOUR_GITHUB_USERNAME -p YOUR_TOKEN
```

> **Note:** Store your token securely. You can also use environment variables:
> ```bash
> conan remote login github YOUR_USERNAME -p $GITHUB_TOKEN
> ```

### 3. Verify Remote Setup

```bash
conan remote list
# Output should show github remote
```

---

## Installing the Package

### Option A: Using conanfile.txt (Simple)

Create `conanfile.txt` in your project:

```ini
[requires]
design-patterns-cpp14/1.0.24@makiolo/stable

[generators]
CMakeDeps
CMakeToolchain
```

Install:
```bash
mkdir build
cd build
conan install .. --remote=github
```

### Option B: Using conanfile.py (Advanced)

Create `conanfile.py`:

```python
from conan import ConanFile
from conan.tools.cmake import cmake_layout

class MyProjectConan(ConanFile):
    name = "myproject"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    requires = "design-patterns-cpp14/1.0.24@makiolo/stable"
    generators = "CMakeDeps", "CMakeToolchain"
    
    def layout(self):
        cmake_layout(self)
```

Install:
```bash
conan install . --build=missing --remote=github
```

---

## Using in CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyProject CXX)

set(CMAKE_CXX_STANDARD 14)

# Find the package (Conan will have generated the files)
find_package(design-patterns-cpp14 REQUIRED CONFIG)

# Create your executable
add_executable(my_app main.cpp)

# Link with the design-patterns library
target_link_libraries(my_app design-patterns-cpp14::design-patterns-cpp14)
```

---

## Complete Example Project

### Directory Structure
```
my_project/
├── conanfile.txt
├── CMakeLists.txt
├── src/
│   └── main.cpp
└── build/  (created by conan)
```

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
project(MyDesignPatternsApp CXX)

set(CMAKE_CXX_STANDARD 14)

find_package(design-patterns-cpp14 REQUIRED CONFIG)

add_executable(app src/main.cpp)
target_link_libraries(app design-patterns-cpp14::design-patterns-cpp14)
```

### src/main.cpp
```cpp
#include <iostream>
#include <factory.h>
#include <memoize.h>

int main() {
    std::cout << "Using design-patterns-cpp14 from GitHub Packages!" << std::endl;
    // Your code using factory.h, memoize.h patterns here
    return 0;
}
```

### Build Steps

```bash
# 1. Install dependencies
mkdir build
cd build
conan install .. --remote=github

# 2. Configure CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# 3. Build
cmake --build .

# 4. Run
./app
```

---

## CI/CD Integration

### GitHub Actions Example

Create `.github/workflows/build.yml`:

```yaml
name: Build with design-patterns-cpp14

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
      
      - name: Install Conan
        run: pip install 'conan>=2.0.0,<3.0.0'
      
      - name: Configure Conan
        run: |
          conan remote add github \
            "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan" \
            -f --insecure
          conan remote login github github-actions -p ${{ secrets.GITHUB_TOKEN }}
      
      - name: Install dependencies
        run: |
          mkdir build && cd build
          conan install .. --remote=github
      
      - name: Build
        run: |
          cd build
          cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
          cmake --build .
```

---

## Available Versions

Check available versions:

```bash
conan search "design-patterns-cpp14/*" --remote=github
```

Or visit: https://github.com/makiolo/design-patterns-cpp14/packages

---

## Troubleshooting

### "401 Unauthorized" Error
- Check your token is correct
- Ensure token has `read:packages` scope
- Try re-login: `conan remote login github YOUR_USERNAME -p YOUR_TOKEN`

### "package not found" Error
- Verify the version exists: `conan search "design-patterns-cpp14/*" --remote=github`
- Check remote is configured: `conan remote list`
- Try: `conan install . --remote=github --build=missing`

### "Connection refused"
- Make sure `--insecure` flag is used or certificate is valid
- Check internet connection
- Verify GitHub status: https://www.githubstatus.com

### Using in Private Repository
If your repo is private, also add `repo` scope to your PAT token.

---

## Environment Variables (CI/CD)

For automated builds, use environment variables instead of storing tokens:

```bash
export CONAN_PASSWORD=${{ secrets.GITHUB_TOKEN }}
export CONAN_USERNAME=github-actions

conan remote add github \
  "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan"
conan remote login github github-actions -p $CONAN_PASSWORD
```

---

## More Information

- [README.md](../README.md) - Main project information
- [docs/DISTRIBUTION.md](DISTRIBUTION.md) - All distribution options
- [docs/INSTALL_FROM_GITHUB.md](INSTALL_FROM_GITHUB.md) - Installing from source

---

**Questions?** Open an issue: https://github.com/makiolo/design-patterns-cpp14/issues
