#include <iostream>
#include <assert.h>
#include <design-patterns/builder.h>

class Base
{
public:
	typedef Builder<Base, const std::string&> Builder;

	// used for template Builder<>
	// define cache behaviour
	static std::string get_data_key(const std::string& key)
	{
		// key for cache
		return key;
	}
	
	Base(const std::string&) { std::cout << "constructor" << std::endl; }
	virtual ~Base() { std::cout << "destruction" << std::endl; }
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

// register implementations to builder
namespace regA
{
	BuilderRegistrator<Base, A, const std::string&> reg;
}
namespace regB
{
	BuilderRegistrator<Base, B, const std::string&> reg;
}

// if you want publish your builder in a DLL, only need
//		template LIBNAME_API Base::Builder;

int main()
{
	// Builder is a factory with cache
	{
		std::shared_ptr<Base> a1 = Base::Builder::instance().get<A>("product1");
		std::shared_ptr<A> a2 = Base::Builder::instance().get<A>("product1");
		std::shared_ptr<Base> a3 = Base::Builder::instance().get(A::KEY(), "product1");

		std::shared_ptr<Base> b1 = Base::Builder::instance().get<B>("product2");
		std::shared_ptr<B> b2 = Base::Builder::instance().get<B>("product2");
		std::shared_ptr<Base> b3 = Base::Builder::instance().get(B::KEY(), "product2");

		assert(a1 == a2);
		assert(a2 == a3);

		assert(b1 == b2);
		assert(b2 == b3);

		assert(a1 != b1);
	}

	/*
	Output valid test:
	
	constructor
	constructor
	destruction
	destruction
	*/

	return(0);
}
