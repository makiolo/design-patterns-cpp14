PROJECT(DESIGN-PATTERNS-CPP14 CXX)
cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(include/)
add_definitions(-std=c++11)
ADD_EXECUTABLE(test_memoize_static tests/test_memoize_static.cpp)
ADD_EXECUTABLE(test_memoize_dynamic tests/test_memoize_dynamic.cpp)
ADD_EXECUTABLE(test_factory_static tests/test_factory_static.cpp)
ADD_EXECUTABLE(test_factory_dynamic tests/test_factory_dynamic.cpp)
ADD_EXECUTABLE(test_command tests/test_command.cpp)

