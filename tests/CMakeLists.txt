cmaki_find_package(boost-headers)
cmaki_find_package(json)

cmaki_library(foo foo.cpp)
cmaki_library(fooA fooA.cpp DEPENDS foo)
cmaki_library(fooB fooB.cpp DEPENDS foo)

cmaki_google_test(factory_dynamic test_factory_dynamic.cpp PTHREADS)
cmaki_google_test(factory_static test_factory_static.cpp PTHREADS)
cmaki_google_test(memoize_dynamic test_memoize_dynamic.cpp PTHREADS)
cmaki_google_test(memoize_static test_memoize_static.cpp PTHREADS)
cmaki_google_test(factory_linking test_factory_linking.cpp DEPENDS foo dl PTHREADS)
cmaki_google_test(repository_static test_repository_static.cpp PTHREADS)
cmaki_google_test(repository_once_static test_repository_once_static.cpp PTHREADS)

