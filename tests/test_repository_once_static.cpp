#include <iostream>
#include <tuple>
#include <map>
#include <functional>
#include <metacommon/common.h>
#include "../memoize.h"

using repo = dp14::repository_once<float, std::string>;

struct foo : dp14::code_once<float, std::string>
{
	explicit foo(const std::string& payload)
	{
		std::cout << "foo: payload = " << payload << std::endl;
		set(1.0f);
	}
};
DEFINE_HASH_CUSTOM(foo, float, 1.0f)

struct reb : dp14::code_once<float, std::string>
{
	explicit reb(const std::string& payload)
	{
		std::cout << "reb: payload = " << payload << std::endl;	
		set(2.0f);
	}
};
DEFINE_HASH_CUSTOM(reb, float, 2.0f)

struct tol : dp14::code_once<float, std::string>
{
	explicit tol(const std::string& payload)
	{
		std::cout << "tol: payload = " << payload << std::endl;
		set(3.0f);
	}
};
DEFINE_HASH_CUSTOM(tol, std::string, "/homie/salon/temperature")

int main()
{
	repo r;
	repo::reg<foo> r1(r);
	repo::reg<reb> r2(r);
	repo::reg<tol> r3(r);
	std::cout << r.execute("/homie/salon/temperature", "24.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "24.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "24.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "25.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "25.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "25.0") << std::endl;
	std::cout << "clearing cache" << std::endl;
	r.clear();
	std::cout << r.execute("/homie/salon/temperature", "24.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "24.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "24.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "25.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "25.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "25.0") << std::endl;
	std::cout << r.execute(1.0f, "register with 1.0f") << std::endl;
	std::cout << r.execute(1.0f, "register with 1.0f") << std::endl;
	std::cout << r.execute(2.0f, "register with 2.0f") << std::endl;
	std::cout << r.execute(2.0f, "register with 2.0f") << std::endl;
	r.clear();
	std::cout << r.execute(2.0f, "register with 2.0f") << std::endl;
	r.clear(2.0f, "register with 2.0f");
	std::cout << r.execute(2.0f, "register with 2.0f") << std::endl;
	return 0;
}
