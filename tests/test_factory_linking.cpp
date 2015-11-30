#include <assert.h>
#include "foo.h"
#include "fooA.h"
#include "fooB.h"
#include <dlfcn.h>

class load_library
{
public:
	load_library(const std::string& libname)
	{
		//_handle = dlopen(libname.c_str(), RTLD_NOW);
		_handle = dlopen(libname.c_str(), RTLD_LAZY);
		if (!_handle)
		{
			fputs (dlerror(), stderr);
			throw std::runtime_error("error loading library");
		}
	}

	~load_library()
	{
		dlclose(_handle);
	}
protected:
	void* _handle;
};

int main()
{
	using namespace foo;
	using namespace fooA;
	using namespace fooB;

	load_library fooA("libfooA.so");
	load_library fooB("libfooB.so");

	// equivalent ways of create A
	std::shared_ptr<Base> a1 = Base::get_factory().create<A>("first parameter", 2);
	std::shared_ptr<A> a2 = Base::get_factory().create<A>("first parameter", 2);
	std::shared_ptr<Base> a3 = Base::get_factory().create(A::KEY(), "first parameter", 2);

	// equivalent ways of create B
	std::shared_ptr<Base> b1 = Base::get_factory().create<B>("first parameter", 2);
	std::shared_ptr<B> b2 = Base::get_factory().create<B>("first parameter", 2);
	std::shared_ptr<Base> b3 = Base::get_factory().create(B::KEY(), "first parameter", 2);

	assert(a1 != a2);
	assert(a3 != b1);
	assert(b1 != b2);

	return(0);
}

