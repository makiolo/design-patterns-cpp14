#include <iostream>
#include <assert.h>
#include <factory.h>
#include <gmock/gmock.h>
class FactoryDynamicTests : testing::Test {};

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

class B : public Base
{
public:
	explicit B(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~B() = default;
};

// no-macros option
namespace std
{
	template <>
	struct hash<B>
	{
		size_t operator()() const
		{
			return std::hash<std::string>()("B");
		}
	};
}

TEST(FactoryDynamicTests, Test1)
{
	Base::factory f;
	Base::factory::registrator<A> reg1(f);
	Base::factory::registrator<B> reg2(f);

	{
		// equivalent ways of create A
		// std::unique_ptr<Base> a1 = f.create_specialized<A>("first parameter", 2);
		// std::unique_ptr<A> a2 = f.create_specialized<A>("first parameter", 2);
		std::unique_ptr<Base> a3 = f.create("A", "first parameter", 2);

		// equivalent ways of create B
		// std::unique_ptr<Base> b1 = f.create_specialized<B>("first parameter", 2);
		// std::unique_ptr<B> b2 = f.create_specialized<B>("first parameter", 2);
		std::unique_ptr<Base> b3 = f.create("B", "first parameter", 2);

		// assert(a1 != a2);
		// assert(a3 != b1);
		// assert(b1 != b2);
		assert(a3 != b3);
	}
}
