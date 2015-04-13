PROJECT(DESIGN-PATTERNS-CPP14 CXX)
cmake_minimum_required(VERSION 2.8)
enable_testing()

INCLUDE_DIRECTORIES(include/)
add_definitions(${EXTRA_DEF})

# Generate .clang_complete for full completation in vim + clang_complete
set(extra_parameters "")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  set(extra_parameters ${extra_parameters} -I${dir})
endforeach()
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY COMPILE_DEFINITIONS)
foreach(dir ${dirs})
  set(extra_parameters ${extra_parameters} -D${dir})
endforeach()
STRING(REGEX REPLACE ";" "\n" extra_parameters "${extra_parameters}")
FILE(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/.clang_complete" "${extra_parameters}\n")

foreach(BBVA_BUILD_TYPE ${CMAKE_BUILD_TYPE})
	INSTALL(    TARGETS ${LIBNAME}
				DESTINATION ${BBVA_BUILD_TYPE}
				CONFIGURATIONS ${BBVA_BUILD_TYPE})
endforeach()

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

ADD_EXECUTABLE(test_fes tests/test_fes.cpp)
ADD_TEST(NAME test_fes COMMAND test_fes WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
