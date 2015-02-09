#include <iostream>
#include <assert.h>
#include <design-patterns/factory.h>

class Base
{
public:
	typedef Factory<Base, const std::string&> Factory;
		
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
		std::shared_ptr<Base> a1 = Base::Factory::instance().create<A>("product1");
		std::shared_ptr<A> a2 = Base::Factory::instance().create<A>("product1");
		std::shared_ptr<Base> a3 = Base::Factory::instance().create(A::KEY(), "product1");
		std::shared_ptr<Base> a4 = Base::Factory::instance().create("A", "product1");

		// equivalent ways of create B
		std::shared_ptr<Base> b1 = Base::Factory::instance().create<B>("product2");
		std::shared_ptr<B> b2 = Base::Factory::instance().create<B>("product2");
		std::shared_ptr<Base> b3 = Base::Factory::instance().create(B::KEY(), "product2");
		std::shared_ptr<Base> b4 = Base::Factory::instance().create("B", "product2");

		assert(a1 != a2);
		assert(a2 != a3);
		assert(a3 != a4);
		assert(a4 != b1);
		assert(b1 != b2);
		assert(b2 != b3);
		assert(b3 != b4);
	}

	/*
	Output valid test:
	
	constructor
	constructor
	constructor
	constructor
	constructor
	constructor
	constructor
	constructor
	destruction
	destruction
	destruction
	destruction
	destruction
	destruction
	destruction
	destruction
	*/

	return(0);
}
