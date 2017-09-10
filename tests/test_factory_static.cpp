#include <iostream>
#include <assert.h>
#include <factory.h>
#include <gmock/gmock.h>
#include <boost/poly_collection/base_collection.hpp>

class FactoryStaticTests : testing::Test {};

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

	virtual void who() const = 0;

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

	virtual void who() const
	{
		std::cout << "is A" << std::endl;
	}
};

class B : public Base
{
public:
	DEFINE_KEY(B)
	explicit B(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~B() = default;

	virtual void who() const
	{
		std::cout << "is B" << std::endl;
	}
};

// register implementations to static factory
namespace regA
{
	Base::factory::registrator<A> reg;
}
namespace regB
{
	Base::factory::registrator<B> reg;
}

TEST(FactoryStaticTests, Test1)
{
	boost::base_collection<Base> polly;
	polly.register_types<A, B>();
	polly.insert( *Base::factory::instance().create(A::KEY(), "first parameter", 2) );
	polly.insert( *Base::factory::instance().create("B", "first parameter", 2) );
	polly.insert( *Base::factory::instance().create(A::KEY(), "first parameter", 2) );
	polly.insert( *Base::factory::instance().create(A::KEY(), "first parameter", 2) );
	polly.insert( *Base::factory::instance().create("B", "first parameter", 2) );
	polly.insert( *Base::factory::instance().create(A::KEY(), "first parameter", 2) );
	polly.insert( *Base::factory::instance().create("B", "first parameter", 2) );
	polly.insert( *Base::factory::instance().create("B", "first parameter", 2) );
	polly.insert( *Base::factory::instance().create("B", "first parameter", 2) );
	for(const auto& elem : polly)
	{
		elem.who();
	}
}

