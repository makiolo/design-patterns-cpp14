#ifndef _FOO_A_H_
#define _FOO_A_H_

#include "foo.h"

#ifdef _WIN32
    #ifdef fooA_EXPORTS
        #define fooA_API __declspec(dllexport)
    #else
        #ifndef fooA_STATIC
            #define fooA_API __declspec(dllimport)
        #else
            #define fooA_API
        #endif
    #endif
#else
    #ifdef fooA_EXPORTS
		#if __GNUC__ >= 4
			#define fooA_API __attribute__((visibility("default")))
		#else
			#define fooA_API
		#endif
    #else
        #define fooA_API
    #endif
#endif

class fooA_API A : public Base
{
public:
	explicit A(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~A() = default;
};
DEFINE_HASH(A)

#endif

