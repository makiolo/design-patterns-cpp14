#include <iostream>
#include <tuple>
#include <map>
#include <functional>
#include <metacommon/common.h>
#include "../factory.h"

using repo = dp14::repository<std::string, std::string>;

struct foo : dp14::code<std::string, std::string>
{
    foo(const std::string& topic, const std::string& payload)
    {
		std::cout << "foo: topic: " << topic << ", payload = " << payload << std::endl;
    }
};
// DEFINE_HASH_CUSTOM(foo, float, 1.0f)
DEFINE_HASH(foo)


struct reb : dp14::code<std::string, std::string>
{
    reb(const std::string& topic, const std::string& payload)
    {
		std::cout << "reb: topic: " << topic << ", payload = " << payload << std::endl;
    }
};
// DEFINE_HASH_CUSTOM(reb, float, 2.0f)
DEFINE_HASH(reb)


struct tol : dp14::code<std::string, std::string>
{
    tol(const std::string& topic, const std::string& payload)
    {
		std::cout << "tol: topic: " << topic << ", payload = " << payload << std::endl;
    }
};
// DEFINE_HASH_CUSTOM(tol, std::string, "/homie/salon/temperature")
// DEFINE_HASH(tol)
namespace std
{
	template <>
	struct hash<tol>
	{
		size_t operator()() const { static size_t h = std::hash<std::string>()("/homie/salon/temperature"); return h; }
	};
}


int main()
{
    repo r;
    repo::reg<foo> r1(r);
    repo::reg<reb> r2(r);
    repo::reg<tol> r3(r);
    r.create("/homie/salon/temperature", "tol", "24.0");

    return 0;
}

