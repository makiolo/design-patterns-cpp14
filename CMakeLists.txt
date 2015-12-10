PROJECT(DESIGN-PATTERNS CXX)
cmake_minimum_required(VERSION 2.8)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmaki/utils.cmake")
ENABLE_MODERN_CPP()
include_directories(metacommon)
add_subdirectory(tests)

