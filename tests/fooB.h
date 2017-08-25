#ifndef _FOO_B_H_
#define _FOO_B_H_

#include "foo.h"

#ifdef _WIN32
    #ifdef fooB_EXPORTS
        #define fooB_API __declspec(dllexport)
    #else
		// no import if you want a load delayed and uncoupled of library
		#define fooB_API
    #endif
#else
    #ifdef fooB_EXPORTS
		#define fooB_API __attribute__((visibility("default")))
    #else
        #define fooB_API
    #endif
#endif

namespace fooB {

class fooB_API B : public foo::Base
{
public:
	DEFINE_KEY(fooB::B)
	explicit B(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~B() = default;
};

}

#endif

