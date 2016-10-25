PROJECT(DESIGN-PATTERNS CXX)
cmake_minimum_required(VERSION 2.8)
include("${CMAKE_SOURCE_DIR}/cmaki/cmaki.cmake")
cmaki_setup()
include_directories(metacommon)
add_subdirectory(tests)

