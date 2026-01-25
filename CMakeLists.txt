project(design-patterns-cpp14 CXX)
cmake_minimum_required(VERSION 3.15)

# Enable modern CMake features
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Conan integration (if using Conan)
if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    include("${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
endif()

# Ensure metacommon headers are available
# If not found, download them
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/metacommon/common.h")
    message(STATUS "Downloading metacommon headers...")
    include(FetchContent)
    FetchContent_Declare(
        metacommon
        GIT_REPOSITORY git@github.com:makiolo/metacommon.git
        GIT_TAG master
        SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/metacommon-src"
    )
    FetchContent_GetProperties(metacommon)
    if(NOT metacommon_POPULATED)
        FetchContent_Populate(metacommon)
        # Create include/metacommon directory if it doesn't exist
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/metacommon")
        # Copy common.h from metacommon root to include/metacommon/
        file(COPY "${metacommon_SOURCE_DIR}/common.h" 
             DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/include/metacommon/")
        message(STATUS "Metacommon headers downloaded successfully")
    endif()
endif()

# Find dependencies (GTest only needed for tests)
find_package(GTest REQUIRED CONFIG)

# Add the library interface (header-only)
add_library(design-patterns-cpp14 INTERFACE)
target_include_directories(design-patterns-cpp14 INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(design-patterns-cpp14 INTERFACE cxx_std_14)

# Enable testing
enable_testing()

# Test libraries
add_library(foo tests/foo.cpp)
target_link_libraries(foo PUBLIC design-patterns-cpp14)

add_library(fooA tests/fooA.cpp)
target_link_libraries(fooA PUBLIC foo design-patterns-cpp14)

add_library(fooB tests/fooB.cpp)
target_link_libraries(fooB PUBLIC foo design-patterns-cpp14)

# Helper function to add tests
function(add_pattern_test test_name test_source)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} 
        design-patterns-cpp14
        GTest::gtest 
        GTest::gtest_main
        ${ARGV2}
    )
    if(UNIX AND NOT APPLE)
        target_link_libraries(${test_name} dl pthread)
    endif()
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Add tests
add_pattern_test(factory_dynamic tests/test_factory_dynamic.cpp)
# NOTE: test_factory_static.cpp requires Boost poly_collection and is disabled
# to keep this library with zero external dependencies
# add_pattern_test(factory_static tests/test_factory_static.cpp)
add_pattern_test(memoize_dynamic tests/test_memoize_dynamic.cpp)
add_pattern_test(memoize_static tests/test_memoize_static.cpp)
add_pattern_test(factory_linking tests/test_factory_linking.cpp foo)
add_pattern_test(repository_static tests/test_repository_static.cpp)
add_pattern_test(repository_once_static tests/test_repository_once_static.cpp)

