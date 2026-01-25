cmake_minimum_required(VERSION 3.15)
project(design-patterns-example CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include Conan's generated CMake files
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# Find the design-patterns package
find_package(design-patterns-cpp14 REQUIRED CONFIG)

# Create example executable
add_executable(example_factory example_factory.cpp)
target_link_libraries(example_factory design-patterns-cpp14::design-patterns-cpp14)

# Alternative: using older style (if using CMakeDeps generator)
# target_include_directories(example_factory PRIVATE ${design-patterns-cpp14_INCLUDE_DIRS})
