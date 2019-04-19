#include <iostream>
#include <sstream>
#include <assert.h>
#include <memoize.h>
#include <gmock/gmock.h>
#include <fstream>

class MemoizeStaticTests : testing::Test {};

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
DEFINE_HASH(A)

class B : public Base
{
public:
	DEFINE_KEY(B)
	explicit B(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~B() = default;
};
DEFINE_HASH(B)

// register implementations to static memoize
namespace regA
{
	Base::memoize::registrator<A> reg;
}
namespace regB
{
	Base::memoize::registrator<B> reg;
}

TEST(MemoizeStaticTests, Test1)
{
	// equivalent ways of get A
	std::shared_ptr<Base> a1 = Base::memoize::instance().get<A>("first parameter", 2);
	std::shared_ptr<A> a2 = Base::memoize::instance().get<A>("first parameter", 2);
	std::shared_ptr<Base> a4 = Base::memoize::instance().get(A::KEY(), "first parameter", 4);

	// equivalent ways of get B
	std::shared_ptr<Base> b1 = Base::memoize::instance().get<B>("first parameter", 2);
	std::shared_ptr<B> b2 = Base::memoize::instance().get<B>("first parameter", 2);
	std::shared_ptr<Base> b4 = Base::memoize::instance().get(B::KEY(), "first parameter", 4);

	assert(a1 == a2);
	assert(a2 != a4);

	assert(b1 == b2);
	assert(b2 != b4);
}

bool exists(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

