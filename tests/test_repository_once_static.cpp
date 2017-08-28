#include <iostream>
#include <tuple>
#include <map>
#include <functional>
#include <metacommon/common.h>
#include "../memoize.h"

using repo = dp14::repository_once<float, std::string>;

struct foo : dp14::code_once<float, std::string>
{
    float execute(const std::string& payload) const override
    {
	std::cout << "foo: payload = " << payload << std::endl;
	return 1.0f;
    }
};
DEFINE_HASH_CUSTOM(foo, float, 1.0f)

struct reb : dp14::code_once<float, std::string>
{   
    float execute(const std::string& payload) const override
    {
    	std::cout << "reb: payload = " << payload << std::endl;
	return 2.0f;
    }
};
DEFINE_HASH_CUSTOM(reb, float, 2.0f)

struct tol : dp14::code_once<float, std::string>
{
    float execute(const std::string& payload) const override
    {
	std::cout << "tol: payload = " << payload << std::endl;
	return 3.0f;
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
	return 0;
}
