#!/bin/bash
# Quick setup guide for using design-patterns-cpp14 from GitHub Packages

cat << 'EOF'

╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║        Using design-patterns-cpp14 from GitHub Packages - Quick Setup       ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝

📋 STEP-BY-STEP SETUP
═════════════════════════════════════════════════════════════════════════════

1️⃣  CREATE GITHUB TOKEN
─────────────────────────────────────────────────────────────────────────────

Go to: https://github.com/settings/tokens/new

Select scopes:
  ✓ read:packages
  ✓ repo

Click "Generate token"
Copy and save it somewhere safe (you'll need it below)

═════════════════════════════════════════════════════════════════════════════

2️⃣  INSTALL CONAN & CMAKE
─────────────────────────────────────────────────────────────────────────────

$ pip install 'conan>=2.0.0,<3.0.0'
$ apt-get install cmake  # or brew install cmake on macOS

═════════════════════════════════════════════════════════════════════════════

3️⃣  CONFIGURE CONAN REMOTE (One-time Setup)
─────────────────────────────────────────────────────────────────────────────

$ conan remote add github \
  "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan"

$ conan remote login github YOUR_GITHUB_USERNAME -p YOUR_TOKEN_HERE

Verify it worked:
$ conan remote list
# Output should show the github remote

═════════════════════════════════════════════════════════════════════════════

4️⃣  CREATE YOUR PROJECT
─────────────────────────────────────────────────────────────────────────────

$ mkdir my_project && cd my_project

Create conanfile.txt:
────────────────────
[requires]
design-patterns-cpp14/1.0.24@makiolo/stable

[generators]
CMakeDeps
CMakeToolchain

Create CMakeLists.txt:
──────────────────────
cmake_minimum_required(VERSION 3.15)
project(MyApp CXX)

set(CMAKE_CXX_STANDARD 14)

find_package(design-patterns-cpp14 REQUIRED CONFIG)

add_executable(myapp main.cpp)
target_link_libraries(myapp design-patterns-cpp14::design-patterns-cpp14)

Create main.cpp:
────────────────
#include <iostream>
#include <factory.h>

int main() {
    std::cout << "Hello from design-patterns-cpp14!" << std::endl;
    return 0;
}

═════════════════════════════════════════════════════════════════════════════

5️⃣  BUILD YOUR PROJECT
─────────────────────────────────────────────────────────────────────────────

$ mkdir build && cd build

$ conan install .. --remote=github

$ cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake

$ cmake --build .

$ ./myapp

═════════════════════════════════════════════════════════════════════════════

✨ DONE! You're using design-patterns-cpp14 from GitHub Packages!

═════════════════════════════════════════════════════════════════════════════

🔑 TOKEN SECURITY TIPS
─────────────────────────────────────────────────────────────────────────────

• DON'T commit your token to git
• Use environment variables in CI/CD:
  
  export GITHUB_TOKEN="ghp_xxxxx"
  conan remote login github github-actions -p $GITHUB_TOKEN

• GitHub Actions automatically provides GITHUB_TOKEN secret
• For personal use, create a fine-grained PAT with minimal scope

═════════════════════════════════════════════════════════════════════════════

🚀 CI/CD INTEGRATION (GitHub Actions)
─────────────────────────────────────────────────────────────────────────────

Create .github/workflows/build.yml:

name: Build
on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v4
        with:
          python-version: '3.11'
      
      - run: pip install cmake 'conan>=2.0.0,<3.0.0'
      
      - run: |
          conan remote add github \
            "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan" \
            -f --insecure
          conan remote login github github-actions -p ${{ secrets.GITHUB_TOKEN }}
      
      - run: mkdir build && cd build && conan install .. --remote=github
      - run: cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
      - run: cd build && cmake --build .

═════════════════════════════════════════════════════════════════════════════

📚 DOCUMENTATION
─────────────────────────────────────────────────────────────────────────────

Full guide:     docs/GITHUB_PACKAGES.md
Example project: examples/consumer_project/
All options:     docs/DISTRIBUTION.md

═════════════════════════════════════════════════════════════════════════════

❓ TROUBLESHOOTING
─────────────────────────────────────────────────────────────────────────────

Problem: "401 Unauthorized"
Solution: 
  • Check your token is correct
  • Ensure it has 'read:packages' scope
  • Try re-login: conan remote login github USERNAME -p TOKEN

Problem: "Package not found"
Solution:
  • Verify version: conan search "design-patterns-cpp14/*" --remote=github
  • Check remote: conan remote list

Problem: "Certificate verify failed"
Solution:
  • Add --insecure flag: conan install .. --remote=github --insecure

═════════════════════════════════════════════════════════════════════════════

Need help? Open an issue: 
https://github.com/makiolo/design-patterns-cpp14/issues

EOF
