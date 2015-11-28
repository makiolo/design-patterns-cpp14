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
	DEFINE_KEY(A)
	explicit A(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~A() = default;
};

}

namespace std {

template <>
struct fooA_API hash<fooA::A>
{
public:
	size_t operator()() const { return std::hash<std::string>()(fooA::A::KEY()); }
};

}


#endif

