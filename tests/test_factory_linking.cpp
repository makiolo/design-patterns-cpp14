#include <assert.h>
#include "foo.h"
#include "fooA.h"
#include "fooB.h"
#include <gmock/gmock.h>
class FactoryLinkingTests : testing::Test {};

#ifdef _WIN32

#include "Windows.h"

class load_library
{
public:
	explicit load_library(const std::string& libname)
	{
		_handle = LoadLibrary((libname + ".dll").c_str());
		if (not _handle)
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

#elif __APPLE__

#include <dlfcn.h>

class load_library
{
public:
	explicit load_library(const std::string& libname)
	{
		_handle = dlopen(("lib" + libname + ".dylib").c_str(), RTLD_NOW); // RTLD_LAZY
		if (not _handle)
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

#elif __linux__

#include <dlfcn.h>

class load_library
{
public:
	explicit load_library(const std::string& libname)
	{
		_handle = dlopen(("lib" + libname + ".so").c_str(), RTLD_NOW); // RTLD_LAZY
		if (not _handle)
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

TEST(FactoryLinkingTests, Test1)
{
	using namespace foo;
	using namespace fooA;
	using namespace fooB;

	load_library fooA("fooA");
	load_library fooB("fooB");
	{
		load_library foo("foo");

		// equivalent ways of create A
		// std::unique_ptr<Base> a1 = Base::get_factory().create_specialized<A>("first parameter", 2);
		// std::unique_ptr<A> a2 = Base::get_factory().create_specialized<A>("first parameter", 33);
		std::unique_ptr<Base> a3 = Base::get_factory().create(A::KEY(), "first parameter", 444);

		// equivalent ways of create B
		// std::unique_ptr<Base> b1 = Base::get_factory().create_specialized<B>("first parameter", 7);
		// std::unique_ptr<B> b2 = Base::get_factory().create_specialized<B>("first parameter", 88);
		std::unique_ptr<Base> b3 = Base::get_factory().create(B::KEY(), "first parameter", 999);

		// assert(a1 != a2);
		// assert(a3 != b1);
		// assert(b1 != b2);
		assert(a3 != b3);
	}
}
