PROJECT(DESIGN-PATTERNS CXX)
cmake_minimum_required(VERSION 2.8)

set(CMAKI_REPOSITORY "http://artifacts.myftp.biz:8080")
include("${CMAKE_SOURCE_DIR}/cmaki/cmaki.cmake")
cmaki_setup()

# no library, is header only

add_subdirectory(tests)

