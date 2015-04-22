// design-patterns-cpp14 by Ricardo Marmolejo García is licensed under a Creative Commons Reconocimiento 4.0 Internacional License.
// http://creativecommons.org/licenses/by/4.0/
//
#ifndef _PATTERNS_COMMON_
#define _PATTERNS_COMMON_

#include <functional>
#include <string>
#include <memory>
#include <map>
#include <exception>

#define DEFINE_KEY(__CLASS__) \
	static const std::string& KEY() { static std::string key = #__CLASS__; return key; } \
	virtual const std::string& getKEY() const { static std::string key = #__CLASS__; return key; } \

template<int...>
struct int_sequence {};

template<int N, int... Is>
struct make_int_sequence : make_int_sequence<N-1, N-1, Is...> {};

template<int... Is>
struct make_int_sequence<0, Is...> : int_sequence<Is...> {};

template<int>
struct placeholder_template
{
	
};

namespace std
{
	template<int N>
	struct is_placeholder< placeholder_template<N> > : integral_constant<int, N+1>
	{
		
	};
}

template <int... Is>
struct seq {};

template <int N, int... Is>
struct gens : gens < N - 1, N - 1, Is... > {};

template <int... Is>
struct gens<0, Is...> : seq < Is... > {};

#endif
