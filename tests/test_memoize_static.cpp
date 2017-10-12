#include <iostream>
#include <sstream>
#include <assert.h>
#include <memoize.h>
#include <gmock/gmock.h>
#include <json.hpp>
#include <fstream>

using json = nlohmann::json;

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

TEST(MemoizeStaticTests, DISABLED_TestJSON)
{
	json j3;
	for(int i = 0; i < 2; ++i)
	{
		// default json
		j3 = R"(
		  {
			"happy": true,
			"happy2": false,
			"pi": 3.141
		  }
		)"_json;
		
		std::ofstream out("example.json");
		out << std::setw(4) << j3 << std::endl;

		if(exists("example.bin"))
		{
			std::basic_ifstream<std::uint8_t> file_bin("example.bin");
			file_bin.seekg(0, std::ios::end);
			size_t len = file_bin.tellg();
			std::uint8_t* buffer = new std::uint8_t[len];
			file_bin.seekg(0, std::ios::beg);
			file_bin.read(buffer, len);
			file_bin.close();
			std::vector<std::uint8_t> v_cbor(len);
			for(size_t j=0;j<len; ++j)
				v_cbor.push_back(buffer[j]);
			std::cout << "read " << v_cbor.size() << " bytes (uint8)" << std::endl;
			// j3 = json::from_cbor(v_cbor);
			j3 = json::from_msgpack(v_cbor);
			delete [] buffer;
		}
		else
		{
			std::ifstream file_json("example.json");
			file_json >> j3;
		}
		std::cout << j3.dump(4) << std::endl;

		std::ofstream o("example.bin");
		// std::vector<std::uint8_t> v_cbor = json::to_cbor(j3);
		std::vector<std::uint8_t> v_cbor = json::to_msgpack(j3);
		size_t len = v_cbor.size();
		std::cout << "write " << v_cbor.size() << " bytes (uint8)" << std::endl;
		std::uint8_t* buffer = new std::uint8_t[len];
		std::copy(v_cbor.begin(), v_cbor.end(), buffer);
		o << buffer;
		delete [] buffer;
	}
}

