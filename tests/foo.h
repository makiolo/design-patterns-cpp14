#ifndef _FOO_H_
#define _FOO_H_

#include <iostream>
#include "../factory.h"

#ifdef _WIN32
    #ifdef foo_EXPORTS
        #define foo_API __declspec(dllexport)
    #else
		#define foo_API __declspec(dllimport)
    #endif
#else
    #ifdef foo_EXPORTS
		#define foo_API __attribute__((visibility("default")))
    #else
        #define foo_API
    #endif
#endif

namespace foo {

class foo_API Base
{
public:
	using factory = dp14::factory<Base, std::string, int>;

	// this let registrations since externals libraries
	static Base::factory& get_factory();

	explicit Base(const std::string& name, int q)
		: _name(name)
		, _q(q)
	{
		std::cout << "constructor " << _name << " - " << _q << std::endl;
	}
	virtual ~Base() { std::cout << "destruction" << std::endl; }

protected:
	std::string _name;
	int _q;
};

}

#endif

