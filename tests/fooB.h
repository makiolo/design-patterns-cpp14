#ifndef _FOO_B_H_
#define _FOO_B_H_

#include "foo.h"

#ifdef _WIN32
    #ifdef fooB_EXPORTS
        #define fooB_API __declspec(dllexport)
    #else
        #ifndef fooB_STATIC
            #define fooB_API __declspec(dllimport)
        #else
            #define fooB_API
        #endif
    #endif
#else
    #ifdef fooB_EXPORTS
		#if __GNUC__ >= 4
			#define fooB_API __attribute__((visibility("default")))
		#else
			#define fooB_API
		#endif
    #else
        #define fooB_API
    #endif
#endif

class fooB_API B : public Base
{
public:
	explicit B(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~B() = default;
};
DEFINE_HASH(B)

#endif

