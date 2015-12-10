cmake_minimum_required(VERSION 2.8)

add_library(foo SHARED foo.cpp)
add_library(fooA SHARED fooA.cpp)
add_library(fooB SHARED fooB.cpp)
target_link_libraries(fooA foo)
target_link_libraries(fooB foo)

CREATE_TEST("test_factory_dynamic" "")
CREATE_TEST("test_factory_static" "")
CREATE_TEST("test_memoize_dynamic" "")
CREATE_TEST("test_memoize_static" "")
CREATE_TEST("test_factory_linking" "")

target_link_libraries(test_factory_linking foo)
IF(NOT WIN32)
	target_link_libraries(test_factory_linking dl)
ENDIF()

