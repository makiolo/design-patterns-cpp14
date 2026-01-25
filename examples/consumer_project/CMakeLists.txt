cmake_minimum_required(VERSION 3.15)
project(ConsumerExample CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the design-patterns-cpp14 package
find_package(design-patterns-cpp14 REQUIRED CONFIG)

# Create example executable
add_executable(example_app main.cpp)

# Link with design-patterns-cpp14
target_link_libraries(example_app PRIVATE design-patterns-cpp14::design-patterns-cpp14)

# Enable all warnings
if(MSVC)
    target_compile_options(example_app PRIVATE /W4)
else()
    target_compile_options(example_app PRIVATE -Wall -Wextra -Wpedantic)
endif()
