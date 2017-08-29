#include <iostream>
#include <tuple>
#include <map>
#include <functional>
#include <metacommon/common.h>
#include "../factory.h"
#include <gmock/gmock.h>
class RepositoryStaticTests : testing::Test {};

using repo = dp14::repository<float, std::string>;

struct foo : dp14::code<float, std::string>
{
	DEFINE_KEY(foo)
	explicit foo(const std::string& payload)
	{
		std::cout << "foo: payload = " << payload << std::endl;
		set(1.0f);
	}
};

struct reb : dp14::code<float, std::string>
{
	DEFINE_KEY(reb)
	explicit reb(const std::string& payload)
	{
		std::cout << "reb: payload = " << payload << std::endl;
		set(2.0f);
	}
};

struct tol : dp14::code<float, std::string>
{
	explicit tol(const std::string& payload)
	{
		std::cout << "tol: payload = " << payload << std::endl;
		set(3.0f);
	}
};
DEFINE_HASH_CUSTOM(tol, std::string, "/homie/salon/temperature");

TEST(RepositoryStaticTests, Test1)
{
	repo r;
	repo::reg<foo> r1(r);
	repo::reg<reb> r2(r);
	repo::reg<tol> r3(r);
	std::cout << r.execute("foo", "foooooooooooo") << std::endl;
	std::cout << r.execute("reb", "reeeeeeeeeeeb") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "24.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "24.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "24.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "25.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "25.0") << std::endl;
	std::cout << r.execute("/homie/salon/temperature", "25.0") << std::endl;
}
