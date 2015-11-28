#ifndef _FOO_A_H_
#define _FOO_A_H_

#include "foo.h"

#ifdef _WIN32
    #ifdef fooA_EXPORTS
        #define fooA_API __declspec(dllexport)
    #else
		#define fooA_API __declspec(dllimport)
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
DEFINE_HASH_API(fooA_API, fooA::A)

#endif

