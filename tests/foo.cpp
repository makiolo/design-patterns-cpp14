#include "foo.h"

// factory with singleton is useful for DLL/plugin systems:
// if you want publish your factory in a DLL, only need:
//		template LIBNAME_API Base::factory;
static dp14::factory<foo::Base, std::string, int> g_factory;

dp14::factory<foo::Base, std::string, int>& get_factory()
{
	return g_factory;
}

