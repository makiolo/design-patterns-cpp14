PROJECT(DESIGN-PATTERNS-CPP14 CXX)
cmake_minimum_required(VERSION 2.8)
enable_testing()

INCLUDE_DIRECTORIES(include/)
add_definitions(${EXTRA_DEF})

ADD_EXECUTABLE(test_memoize_static tests/test_memoize_static.cpp)
ADD_TEST(NAME test_memoize_static COMMAND test_memoize_static WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(test_memoize_dynamic tests/test_memoize_dynamic.cpp)
ADD_TEST(NAME test_memoize_dynamic COMMAND test_memoize_dynamic WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(test_factory_static tests/test_factory_static.cpp)
ADD_TEST(NAME test_factory_static COMMAND test_factory_static WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(test_factory_dynamic tests/test_factory_dynamic.cpp)
ADD_TEST(NAME test_factory_dynamic COMMAND test_factory_dynamic WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(test_command tests/test_command.cpp)
ADD_TEST(NAME test_command COMMAND test_command WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(test_leader_follower tests/test_leader_follower.cpp)
ADD_TEST(NAME test_leader_follower COMMAND test_leader_follower WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
