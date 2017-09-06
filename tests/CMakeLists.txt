cmake_minimum_required(VERSION 3.0)

cmaki2_library(foo foo.cpp)
cmaki2_library(fooA fooA.cpp DEPENDS foo)
cmaki2_library(fooB fooB.cpp DEPENDS foo)

cmaki2_gtest(factory_dynamic test_factory_dynamic.cpp PTHREADS)
cmaki2_gtest(factory_static test_factory_static.cpp PTHREADS)
cmaki2_gtest(memoize_dynamic test_memoize_dynamic.cpp PTHREADS)
cmaki2_gtest(memoize_static test_memoize_static.cpp PTHREADS)
cmaki2_gtest(factory_linking test_factory_linking.cpp DEPENDS foo dl PTHREADS)
cmaki2_gtest(repository_static test_repository_static.cpp PTHREADS)
cmaki2_gtest(repository_once_static test_repository_once_static.cpp PTHREADS)
