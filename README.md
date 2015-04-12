# Design patterns C++11/14 [![Build Status](https://travis-ci.org/makiolo/design-patterns-cpp14.svg?branch=master)](https://travis-ci.org/makiolo/design-patterns-cpp14)

This is a header-only library with some of the most common design patterns implemmented in C++11/14.

## Design guidelines in design-patterns-cpp14

* Can assume variadic templates supported by your compiler.
* Use perfect forwarding and new features from C++11/14 when possible.
* Prefer header only code, but it is not a must.

## Quality assurance

* Code tested on last versions of gcc, clang and Visual Studio.
* Test cases relationated with problems crossing boundaries of dynamic libraries.

## License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Licencia de Creative Commons" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" href="http://purl.org/dc/dcmitype/Text" property="dct:title" rel="dct:type">design-patterns-cpp14</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="https://github.com/makiolo/design-patterns-cpp14" property="cc:attributionName" rel="cc:attributionURL">Ricardo Marmolejo García</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Reconocimiento 4.0 Internacional License</a>.

## Contributing

The source for design-patterns-cpp14 is held at [design-patterns-cpp14](https://github.com/makiolo/design-patterns-cpp14) github.com site.

To report an issue, use the [design-patterns-cpp14 issue tracker](https://github.com/makiolo/design-patterns-cpp14/issues) at github.com.

## Using design-patterns-cpp14

### Compile design-patterns-cpp14
It's a header-only library. Only need an include.

### Compile tests
You will need cmake (and a compiler).

```
$ git clone https://github.com/makiolo/design-patterns-cpp14.git
$ mkdir build
$ cd build
$ cmake ..
$ make (in unix) or compile generated solution (in windows)
```

### Example factory
```CPP
#include <iostream>
#include <assert.h>
#include <design-patterns/factory.h>

class Base
{
public:
	using Factory = dp14::Factory<Base, const std::string&, const int&>;

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
	Base::Factory factory;
	Base::Factory::Registrator<A> reg1(factory);
	Base::Factory::Registrator<B> reg2(factory);
	
	{
		// equivalent ways of create A
		std::shared_ptr<Base> a1 = factory.create<A>("first parameter", 2);
		std::shared_ptr<A> a2 = factory.create<A>("first parameter", 2);
		std::shared_ptr<Base> a3 = factory.create(A::KEY(), "first parameter", 2);
		std::shared_ptr<Base> a4 = factory.create("A", "first parameter", 2);

		// equivalent ways of create B
		std::shared_ptr<Base> b1 = factory.create<B>("first parameter", 2);
		std::shared_ptr<B> b2 = factory.create<B>("first parameter", 2);
		std::shared_ptr<Base> b3 = factory.create(B::KEY(), "first parameter", 2);
		std::shared_ptr<Base> b4 = factory.create("B", "first parameter", 2);

		assert(a1 != a2);
		assert(a2 != a3);
		assert(a3 != a4);
		assert(a4 != b1);
		assert(b1 != b2);
		assert(b2 != b3);
		assert(b3 != b4);
	}

	return(0);
}
```

### Example memoize (factory + cache)
```CPP
#include <iostream>
#include <sstream>
#include <assert.h>
#include <design-patterns/memoize.h>

class Base
{
public:
	using Memoize = dp14::Memoize<Base, const std::string&, const int&>;
	
	// used for template Memoize<>
	// define cache behaviour
	static std::string get_data_key(const std::string& key_impl, const std::string& name, int n)
	{
		// key for cache
		std::stringstream ss;
		ss << key_impl << "_" << name << "_" << n;
		return ss.str();
	}
	
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
	Base::Memoize memoize;
	Base::Memoize::Registrator<A> reg1(memoize);
	Base::Memoize::Registrator<B> reg2(memoize);
	
	{
		// equivalent ways of get A
		std::shared_ptr<Base> a1 = memoize.get<A>("first parameter", 2);
		std::shared_ptr<A> a2 = memoize.get<A>("first parameter", 2);
		std::shared_ptr<Base> a3 = memoize.get(A::KEY(), "first parameter", 4);
		std::shared_ptr<Base> a4 = memoize.get("A", "first parameter", 4);

		// equivalent ways of get B
		std::shared_ptr<Base> b1 = memoize.get<B>("first parameter", 2);
		std::shared_ptr<B> b2 = memoize.get<B>("first parameter", 2);
		std::shared_ptr<Base> b3 = memoize.get(B::KEY(), "first parameter", 4);
		std::shared_ptr<Base> b4 = memoize.get("B", "first parameter", 4);
		
		assert(a1 == a2);
		assert(a3 == a4);
		assert(a2 != a4);

		assert(b1 == b2);
		assert(b3 == b4);
		assert(b2 != b4);
	}

	return(0);
}
```
