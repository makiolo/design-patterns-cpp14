PROJECT(DESIGN-PATTERNS-CPP14 CXX)
cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(include/)
ADD_EXECUTABLE(test_builder tests/test_builder.cpp)
ADD_EXECUTABLE(test_factory tests/test_factory.cpp)
