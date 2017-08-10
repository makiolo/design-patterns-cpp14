project(design-patterns-cpp14 CXX)
cmake_minimum_required(VERSION 3.0)

set(CMAKI_REPOSITORY "http://artifacts.myftp.biz:8080")
include("${CMAKE_SOURCE_DIR}/node_modules/cmaki/cmaki.cmake")
cmaki_setup()

add_subdirectory(tests)

