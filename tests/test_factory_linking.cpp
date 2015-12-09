#include <assert.h>
#include "foo.h"
#include "fooA.h"
#include "fooB.h"

#ifdef _WIN32

#include "Windows.h"

class load_library
{
public:
	load_library(const std::string& libname)
	{
		_handle = LoadLibrary((libname + ".dll").c_str());
		if (!_handle)
		{
			throw std::runtime_error("error loading library");
		}
	}

	~load_library()
	{
		FreeLibrary(_handle);
	}
protected:
	HMODULE _handle;
};

#else

#include <dlfcn.h>

class load_library
{
public:
	load_library(const std::string& libname)
	{
		_handle = dlopen(("lib" + libname + ".so").c_str(), RTLD_NOW); // RTLD_LAZY
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

#endif

int main()
{
	using namespace foo;
	using namespace fooA;
	using namespace fooB;

	load_library fooA("fooA");
	load_library fooB("fooB");

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

