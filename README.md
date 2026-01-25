# Design patterns C++14
gcc 4.9 / clang 3.6: [![Build Status](https://travis-ci.org/makiolo/design-patterns-cpp14.svg?branch=master)](https://travis-ci.org/makiolo/design-patterns-cpp14)

MSVC 2015: [![Build status](https://ci.appveyor.com/api/projects/status/3ouxeirkgwi8nmyq?svg=true)](https://ci.appveyor.com/project/makiolo/design-patterns-cpp14)

Quality: [![Codacy Badge](https://api.codacy.com/project/badge/Grade/6d20e7024cd0436a935df183c9951a22)](https://www.codacy.com/app/makiolo/design-patterns-cpp14?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=makiolo/design-patterns-cpp14&amp;utm_campaign=Badge_Grade) [![codecov](https://codecov.io/gh/makiolo/design-patterns-cpp14/branch/master/graph/badge.svg)](https://codecov.io/gh/makiolo/design-patterns-cpp14)

This is a header-only library with some of the most common design patterns implemmented in C++11/14.

## Design guidelines in design-patterns-cpp14

* Can assume variadic templates supported by your compiler.
* Use perfect forwarding and new features from C++11/14 when possible.
* Prefer header only code, but it is not a must.
* Allocations and deallocations of memory are centralized in a allocator selectable by client. (TODO: now allocator is FSBAllocator, and require some change.)

## Quality assurance

* Code tested in travis on gcc (4.7, 4.8, 4.9), clang (3.3, 3.4 and 3.6) and Visual Studio (2013).
* Test cases relationated with problems crossing boundaries of dynamic libraries.
* Side dark is optional (no macros, no singletons).

## License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Licencia de Creative Commons" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" href="http://purl.org/dc/dcmitype/Text" property="dct:title" rel="dct:type">design-patterns-cpp14</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="https://github.com/makiolo/design-patterns-cpp14" property="cc:attributionName" rel="cc:attributionURL">Ricardo Marmolejo García</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Reconocimiento 4.0 Internacional License</a>.

## Contributing

The source for design-patterns-cpp14 is held at [design-patterns-cpp14](https://github.com/makiolo/design-patterns-cpp14) github.com site.

To report an issue, use the [design-patterns-cpp14 issue tracker](https://github.com/makiolo/design-patterns-cpp14/issues) at github.com.

## Using design-patterns-cpp14 (As a Consumer)

### Quick Start with Conan (5 minutes)

**Prerequisites:** Conan 2.0+, CMake 3.15+

#### 1. Create a `conanfile.txt` in your project:

```ini
[requires]
design-patterns-cpp14/1.0.24

[generators]
CMakeDeps
CMakeToolchain
```

#### 2. Install dependencies:

```bash
mkdir build && cd build
conan install .. --remote=github
```

#### 3. Configure and build:

```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build .
```

#### 4. Use in your C++ code:

```cpp
#include <dp14/factory.h>
#include <dp14/memoize.h>

// Your code here...
```

### Installation Methods

#### Option 1: Using Conan (Recommended)

**One-time setup:**
```bash
conan remote add github "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan"
conan remote login github YOUR_USERNAME -p YOUR_TOKEN
```

**In your project:** Create `conanfile.txt` as shown above.

**See [docs/GITHUB_PACKAGES_SETUP.md](docs/GITHUB_PACKAGES_SETUP.md) for complete setup with screenshots**

#### Option 2: Copy Headers Directly

For simple projects, just copy the `include/` folder to your project and add it to your CMake:

```cmake
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
```

#### Option 3: Build from Source

Clone and create Conan package locally:

```bash
git clone https://github.com/makiolo/design-patterns-cpp14.git
cd design-patterns-cpp14
conan create . --build=missing
```

### What You Get

This is a **header-only library** with:

- ✅ **Factory pattern**: Create objects of different types dynamically
- ✅ **Memoize pattern**: Cache objects to avoid recreating them
- ✅ **Zero external dependencies**: Uses only C++ standard library
- ✅ **C++14 compatible**: Works with gcc 4.9+, clang 3.6+, MSVC 2015+
- ✅ **Cross-platform**: Linux, macOS, Windows

### Documentation for Consumers

| What you need | Link |
|---|---|
| **Quick start (5 min)** | [docs/GITHUB_PACKAGES_SETUP.md](docs/GITHUB_PACKAGES_SETUP.md) |
| **Complete usage guide** | [docs/GITHUB_PACKAGES.md](docs/GITHUB_PACKAGES.md) |
| **All distribution options** | [docs/DISTRIBUTION.md](docs/DISTRIBUTION.md) |
| **Example project** | [examples/consumer_project/](examples/consumer_project/) |
### Naming implementations
* *option 1*: use DEFINE_KEY(classname or anything) within the class
* *option 2*: use DEFINE_HASH(classname well qualified) outside of class
* *option 3*: specialization of std::hash<T>. This is equivalent to option 2 but without use macros:
```CPP
namespace std {
	template <>
	struct hash<MyClass>
	{
		size_t operator()() const
		{
			return std::hash<std::string>()("MyClass");
		}
	};
}
```
### Example factory
```CPP
#include <iostream>
#include <assert.h>
#include <dp14/factory.h>

class Base
{
public:
	using factory = dp14::factory<Base, std::string, int>;

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

class A : public Base
{
public:
	DEFINE_KEY(A)
	explicit A(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~A() = default;
};
DEFINE_HASH(A)

// if you dont like macro DEFINE_KEY(class), can use this:
class B : public Base
{
public:
	explicit B(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~B() = default;
};

namespace std {
	template <>
	struct hash<B>
	{
		size_t operator()() const
		{
			return std::hash<std::string>()("B");
		}
	};
}

int main()
{
	Base::factory factory;
	Base::factory::registrator<A> reg1(factory);
	Base::factory::registrator<B> reg2(factory);

	{
		// equivalent ways of create A
		std::shared_ptr<Base> a1 = factory.create<A>("first parameter", 2);
		std::shared_ptr<A> a2 = factory.create<A>("first parameter", 2);
		std::shared_ptr<Base> a3 = factory.create("A", "first parameter", 2);

		// equivalent ways of create B
		std::shared_ptr<Base> b1 = factory.create<B>("first parameter", 2);
		std::shared_ptr<B> b2 = factory.create<B>("first parameter", 2);
		std::shared_ptr<Base> b3 = factory.create("B", "first parameter", 2);

		assert(a1 != a2);
		assert(a3 != b1);
		assert(b1 != b2);
	}

	return(0);
}
```

### Example memoize (factory + cache)
```CPP
#include <iostream>
#include <sstream>
#include <assert.h>
#include <dp14/memoize.h>

class Base
{
public:
	using memoize = dp14::memoize<Base, std::string, int>;

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

class A : public Base
{
public:
	DEFINE_KEY(A)
	explicit A(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~A() = default;
};

class B : public Base
{
public:
	DEFINE_KEY(B)
	explicit B(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~B() = default;
};

int main()
{
	Base::memoize m;
	Base::memoize::registrator<A> reg1(m);
	Base::memoize::registrator<B> reg2(m);

	{
		std::shared_ptr<Base> a1 = m.get<A>("first parameter", 2);
		assert( m.exists<A>("first parameter", 2) == true );
	}
	assert( m.exists<A>("first parameter", 2) == false );

	{
		std::shared_ptr<Base> a1 = m.get<A>("first parameter", 2);
		std::shared_ptr<A> a2 = m.get<A>("first parameter", 2);
		assert(a1 == a2);

		std::shared_ptr<Base> a3 = m.get("A", "first parameter", 4);
		assert(a2 != a3);

		std::shared_ptr<Base> b1 = m.get<B>("first parameter", 2);
		std::shared_ptr<B> b2 = m.get<B>("first parameter", 2);
		assert(b1 == b2);

		std::shared_ptr<Base> b3 = m.get("B", "first parameter", 4);
		assert(b2 != b3);

		assert( m.exists<A>("first parameter", 2) == true );
	}
	assert( m.exists<A>("first parameter", 2) == false );

	return(0);
}
```
