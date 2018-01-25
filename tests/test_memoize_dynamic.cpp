#include <iostream>
#include <sstream>
#include <assert.h>
#include <memoize.h>
#include <gmock/gmock.h>
class MemoizeDynamicTests : testing::Test {};

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

TEST(MemoizeDynamicTests, Test1)
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

		std::shared_ptr<Base> a3 = m.get(A::KEY(), "first parameter", 4);
		assert(a2 != a3);

		std::shared_ptr<Base> b1 = m.get<B>("first parameter", 2);
		std::shared_ptr<B> b2 = m.get<B>("first parameter", 2);
		assert(b1 == b2);

		std::shared_ptr<Base> b3 = m.get(B::KEY(), "first parameter", 4);
		assert(b2 != b3);

		assert( m.exists<A>("first parameter", 2) == true );
	}
	assert( m.exists<A>("first parameter", 2) == false );
}

