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

// specialization std::hash<Base>
namespace std
{
	template<>
	class hash<Base>
	{
	public:
		size_t operator()(const std::string& implementation, std::string& name, int n) const
		{
			size_t h1 = std::hash<std::string>()(implementation);
			size_t h2 = std::hash<std::string>()(name);
			size_t h3 = std::hash<int>()(n);
			return h1 ^ (h2 ^ (h3 << 1) << 1);
		}
	};
}

// register implementations to static memoize
namespace regA
{
	Base::Memoize::Registrator<A> reg;
}
namespace regB
{
	Base::Memoize::Registrator<B> reg;
}

// Memoize with singleton is useful for DLL/plugin systems:
// if you want publish your Memoize in a DLL, only need:
//		template LIBNAME_API Base::Memoize;

int main()
{
	{
		// equivalent ways of get A
		std::shared_ptr<Base> a1 = Base::Memoize::instance().get<A>("first parameter", 2);
		std::shared_ptr<A> a2 = Base::Memoize::instance().get<A>("first parameter", 2);
		std::shared_ptr<Base> a3 = Base::Memoize::instance().get(A::KEY(), "first parameter", 4);
		std::shared_ptr<Base> a4 = Base::Memoize::instance().get("A", "first parameter", 4);

		// equivalent ways of get B
		std::shared_ptr<Base> b1 = Base::Memoize::instance().get<B>("first parameter", 2);
		std::shared_ptr<B> b2 = Base::Memoize::instance().get<B>("first parameter", 2);
		std::shared_ptr<Base> b3 = Base::Memoize::instance().get(B::KEY(), "first parameter", 4);
		std::shared_ptr<Base> b4 = Base::Memoize::instance().get("B", "first parameter", 4);
	
		assert(a1 == a2);
		assert(a3 == a4);
		assert(a2 != a4);

		assert(b1 == b2);
		assert(b3 == b4);
		assert(b2 != b4);
	}
	
	return(0);
}
