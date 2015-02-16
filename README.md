# Design patterns C++11/14

This is a header-only library with some of the most common design patterns implemmented in C++11/14.

## Design guidelines in design-patterns-cpp14

* Can assume variadic templates supported by your compiler.
* Use perfect forwarding and new features from C++11/14 when possible.
* Prefer header only code, but it is not a must.

## Quality assurance

* Code tested on last versions of gcc, clang and Visual Studio.
* Test cases relationated with problems crossing boundaries of dynamic libraries.

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
#include <assert.h>
#include <design-patterns/factory.h>

class Base
{
public:
	typedef Factory<Base, const std::string&> Factory;
		
	Base(const std::string&) { ; }
	virtual ~Base() { ; }
};

class A : public Base
{
public:
	DEFINE_KEY(A)
	A(const std::string& data) : Base(data) { ; }  
	virtual ~A() = default;
};

class B : public Base
{
public:
	DEFINE_KEY(B)
	B(const std::string& data) : Base(data) { ; }
	virtual ~B() = default;
};

// register implementations to factory
namespace regA
{
	FactoryRegistrator<Base, A, const std::string&> reg;
}
namespace regB
{
	FactoryRegistrator<Base, B, const std::string&> reg;
}

// if you want publish your factory in a DLL, only need
//		template LIBNAME_API Base::Factory;

int main()
{
	// factory only is a simple creator from type or from string
	{
		// equivalent ways of create A
		std::shared_ptr<Base> a1 = Base::Factory::instance().create<A>("first parameter");
		std::shared_ptr<A> a2 = Base::Factory::instance().create<A>("first parameter");
		std::shared_ptr<Base> a3 = Base::Factory::instance().create(A::KEY(), "first parameter");
		std::shared_ptr<Base> a4 = Base::Factory::instance().create("A", "first parameter");
    
		// equivalent ways of create B
		std::shared_ptr<Base> b1 = Base::Factory::instance().create<B>("first parameter");
		std::shared_ptr<B> b2 = Base::Factory::instance().create<B>("first parameter");
		std::shared_ptr<Base> b3 = Base::Factory::instance().create(B::KEY(), "first parameter");
		std::shared_ptr<Base> b4 = Base::Factory::instance().create("B", "first parameter");
    
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
### License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Licencia de Creative Commons" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" href="http://purl.org/dc/dcmitype/Text" property="dct:title" rel="dct:type">design-patterns-cpp14</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="https://github.com/makiolo/design-patterns-cpp14" property="cc:attributionName" rel="cc:attributionURL">Ricardo Marmolejo García</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Reconocimiento 4.0 Internacional License</a>.
