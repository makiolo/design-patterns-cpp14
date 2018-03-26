project(design-patterns-cpp14 CXX)
cmake_minimum_required(VERSION 3.0)
include(cmaki)

cmaki_find_package(boost-headers)
cmaki_find_package(json)

cmaki_library(foo tests/foo.cpp)
cmaki_library(fooA tests/fooA.cpp DEPENDS foo)
cmaki_library(fooB tests/fooB.cpp DEPENDS foo)

cmaki_google_test(factory_dynamic tests/test_factory_dynamic.cpp PTHREADS)
cmaki_google_test(factory_static tests/test_factory_static.cpp PTHREADS)
cmaki_google_test(memoize_dynamic tests/test_memoize_dynamic.cpp PTHREADS)
cmaki_google_test(memoize_static tests/test_memoize_static.cpp PTHREADS)
cmaki_google_test(factory_linking tests/test_factory_linking.cpp DEPENDS foo dl PTHREADS)
cmaki_google_test(repository_static tests/test_repository_static.cpp PTHREADS)
cmaki_google_test(repository_once_static tests/test_repository_once_static.cpp PTHREADS)

