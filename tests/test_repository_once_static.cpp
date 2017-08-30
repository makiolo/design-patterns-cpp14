#include <iostream>
#include <tuple>
#include <map>
#include <functional>
#include <metacommon/common.h>
#include "../memoize.h"
#include <gmock/gmock.h>
class RepositoryOnceStaticTests : testing::Test {};

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

TEST(RepositoryOnceStaticTests, Test1)
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
}

TEST(RepositoryOnceStaticTests, TestMemoryLeak)
{
	// example memory leak
	int* pn = new int(123);
	std::cout << "leak test number: " << *pn << std::endl;
	ASSERT_EQ(*pn, 123);
	pn = new int(567);
	std::cout << "leak test number: " << *pn << std::endl;
	ASSERT_EQ(*pn, 567);
	
}

TEST(RepositoryOnceStaticTests, TestMemoryLeak2)
{
	char *x = malloc(100); /* or, in C++, "char *x = new char[100] */
}
