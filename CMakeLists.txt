project(design-patterns-cpp14 CXX)
cmake_minimum_required(VERSION 3.0)

set(CMAKI_REPOSITORY "http://artifacts.myftp.biz:8080")
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../cmaki")
	include("${CMAKE_CURRENT_LIST_DIR}/../cmaki/cmaki.cmake")
else()
	include("${CMAKE_CURRENT_LIST_DIR}/node_modules/cmaki/cmaki.cmake")
endif()
cmaki_setup()

add_subdirectory(tests)

