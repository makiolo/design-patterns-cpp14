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
