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
		std::shared_ptr<Base> a1 = memoize.get<A>("first parameter", 2);
		assert( memoize.exists<A>("first parameter", 2) == true );
	}
	assert( memoize.exists<A>("first parameter", 2) == false );

	{
		std::shared_ptr<Base> a1 = memoize.get<A>("first parameter", 2);
		std::shared_ptr<A> a2 = memoize.get<A>("first parameter", 2);
		assert(a1 == a2);

		std::shared_ptr<Base> a3 = memoize.get("A", "first parameter", 4);
		assert(a2 != a3);

		std::shared_ptr<Base> b1 = memoize.get<B>("first parameter", 2);
		std::shared_ptr<B> b2 = memoize.get<B>("first parameter", 2);
		assert(b1 == b2);

		std::shared_ptr<Base> b3 = memoize.get("B", "first parameter", 4);
		assert(b2 != b3);

		assert( memoize.exists<A>("first parameter", 2) == true );
	}
	assert( memoize.exists<A>("first parameter", 2) == false );

	return(0);
}
