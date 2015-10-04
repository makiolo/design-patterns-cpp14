PROJECT(DESIGN-PATTERNS CXX)
cmake_minimum_required(VERSION 2.8)
enable_testing()

IF(NOT DEFINED CMAKE_MODULE_PATH)
	set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmaki)
ENDIF()

include(utils)
ENABLE_MODERN_CPP()

add_subdirectory(tests)

