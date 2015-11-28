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

namespace fooB {

class fooB_API B : public foo::Base
{
public:
	DEFINE_KEY(B)
	explicit B(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~B() = default;
};

}

namespace std {

template <>
struct fooB_API hash<fooB::B>
{
public:
	size_t operator()() const { return std::hash<std::string>()(fooB::B::KEY()); }
};

}

#endif

