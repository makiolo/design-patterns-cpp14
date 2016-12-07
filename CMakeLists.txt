project(design-patterns-cpp14 CXX)
cmake_minimum_required(VERSION 3.2.2)

set(CMAKI_REPOSITORY "http://artifacts.myftp.biz:8080")
include("${CMAKE_SOURCE_DIR}/cmaki/cmaki.cmake")
cmaki_setup()

# no library, is header only

add_subdirectory(tests)
