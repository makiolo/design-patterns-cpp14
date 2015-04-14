// annotations python style in C++
// author: Ricardo Marmolejo Garcia
#include <iostream>
#include <functional>
#include <string>
#include <sstream>

// http://blog.sigfpe.com/2006/08/you-could-have-invented-monads-and.html
// http://stackoverflow.com/questions/44965/what-is-a-monad

using leaf = std::function<std::string()>;
using composite = std::function<leaf(const leaf&)>;
// aggregator convierte una lista de leaf en un solo leaf
// aggregator convierte una lista de composites en un solo composite

leaf operator+=(const composite& a, const leaf& b)
{
	return a(b);
}

composite repeat(int n)
{
	return [=](const leaf& f)
	{
		return [&]()
		{
			std::stringstream ss;
			for(int i=0; i<n; ++i)
			{
				ss << f() << std::endl;
			}
			return ss.str();
		};
	};
}

composite tag(const std::string& tag)
{
	return [=](const leaf& f)
	{
		return [&]() {return "<" + tag + ">" + f() + "</" + tag + ">";};
	};
}

class Generator
{
public:
	Generator(){ ; }
	~Generator(){ ; }
	
	std::string generate()
	{
		auto t = tag("html");
		
		return (t += tag("html") += t += tag("body") += tag("b") += repeat(5) += [](){return "Hello world!";})();
	}
};

int main(int argc, const char *argv[])
{
	Generator p;
	std::cout << p.generate() << std::endl;
	return 0;
}

