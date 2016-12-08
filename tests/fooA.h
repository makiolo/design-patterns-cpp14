#ifndef _FOO_A_H_
#define _FOO_A_H_

#include "foo.h"

#ifdef _WIN32
    #ifdef fooA_EXPORTS
        #define fooA_API __declspec(dllexport)
    #else
	// no import if you want a load delayed and uncoupled of library
	#define fooA_API
    #endif
#else
    #ifdef fooA_EXPORTS
	#define fooA_API __attribute__((visibility("default")))
    #else
        #define fooA_API
    #endif
#endif

namespace fooA {

class fooA_API A : public foo::Base
{
public:
	DEFINE_KEY(fooA::A)
	explicit A(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~A() = default;
};

}

#endif

