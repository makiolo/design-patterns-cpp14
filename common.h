#ifndef _META_COMMON_
#define _META_COMMON_

#include <functional>
#include <string>
#include <memory>
#include <unordered_map>
#include <exception>

#define DEFINE_KEY(__CLASS__) \
	static const std::string& KEY() { static std::string key = #__CLASS__; return key; } \
	virtual const std::string& getKEY() const { static std::string key = #__CLASS__; return key; } \

#define DEFINE_HASH(__CLASS__)  \
	namespace std {             \
	template <>                 \
	struct hash<__CLASS__>      \
	{ size_t operator()() const { static size_t h = std::hash<std::string>()(__CLASS__::KEY()); return h; }	}; }			\

#define DEFINE_HASH_API(__API__, __CLASS__)  \
	namespace std {             \
	template <>                 \
	struct __API__ hash<__CLASS__>      \
	{ size_t operator()() const { static size_t h = std::hash<std::string>()(__CLASS__::KEY()); return h; }	}; }			\

template <int...>
struct int_sequence
{
};

template <int N, int... Is>
struct make_int_sequence : make_int_sequence<N - 1, N - 1, Is...>
{
};

template <int... Is>
struct make_int_sequence<0, Is...> : int_sequence<Is...>
{
};

template <int>
struct placeholder_template
{
};

namespace std
{
template <int N>
struct is_placeholder<placeholder_template<N>> : integral_constant<int, N + 1>
{
};
}

template <int... Is>
struct seq
{
};

template <int N, int... Is>
struct gens : gens<N - 1, N - 1, Is...>
{
};

template <int... Is>
struct gens<0, Is...> : seq<Is...>
{
};

namespace dp14
{

template <typename T>
class hash
{
public:
	template <typename... Args>
	size_t operator()(Args&&... args) const
	{
		size_t h = 0;
		_hash_forwarding(h, std::forward<Args>(args)...);
		return h;
	}

protected:
	template <typename U>
	void _combine_hash(size_t& seed, U&& x) const
	{
		seed ^= std::hash<U>()(std::forward<U>(x)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template <typename U, typename... Args>
	void _hash_forwarding(size_t& h, U&& parm, Args&&... args) const
	{
		_combine_hash<U>(h, std::forward<U>(parm));
		_hash_forwarding(h, std::forward<Args>(args)...);
	}

	template <typename U>
	void _hash_forwarding(size_t& h, U&& parm) const
	{
		_combine_hash<U>(h, std::forward<U>(parm));
	}
};
}

#endif
