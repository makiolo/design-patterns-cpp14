#include <iostream>
#include <sstream>
#include <assert.h>
#include <design-patterns/memoize.h>

class Base
{
public:
	using Memoize = dp14::Memoize<Base, std::string, int>;

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
	explicit A(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~A() = default;
};
DEFINE_HASH(A)

class B : public Base
{
public:
	explicit B(const std::string& name, int q) : Base(name, q) { ; }
	virtual ~B() = default;
};
DEFINE_HASH(B)

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
