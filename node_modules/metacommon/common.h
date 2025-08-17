#ifndef _META_COMMON_
#define _META_COMMON_

#include <functional>
#include <string>
#include <memory>
#include <unordered_map>
#include <exception>
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <tuple>

#ifndef STATIC_MULTITHREAD
#if defined(__clang__)
#define STATIC_MULTITHREAD static
#elif defined(__GNUC__) || defined(__GNUG__)
#define STATIC_MULTITHREAD static __thread
#elif defined(_MSC_VER)
#define STATIC_MULTITHREAD __declspec(thread) static
#endif
#endif

#if __cplusplus < 201402L && 0

namespace std {

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}

#endif

namespace ctti {

// http://stackoverflow.com/a/15863804

// helper function
constexpr unsigned c_strlen( char const* str, unsigned count = 0 )
{
    return ('\0' == str[0]) ? count : c_strlen(str+1, count+1);
}

// helper "function" struct
template < char t_c, char... tt_c >
struct rec_print
{
    static void print()
    {
        std::cout << t_c;
        rec_print < tt_c... > :: print ();
    }
};

template < char t_c >
struct rec_print < t_c >
{
    static void print()
    {
        std::cout << t_c;
    }
};

// helper "function" struct
template < char t_c, char... tt_c >
struct rec_get
{
    static void get(std::stringstream& ss)
    {
        ss << t_c;
        rec_get < tt_c... > :: get (ss);
    }
};

template < char t_c >
struct rec_get < t_c >
{
    static void get(std::stringstream& ss)
    {
        ss << t_c;
    }
};

template < char t_c, char... tt_c >
struct rec_hash
{
    static constexpr size_t hash(size_t seed)
    {
        return rec_hash <tt_c...>::hash(seed * 33 ^ static_cast<unsigned>(t_c));
    }
};

template < char t_c >
struct rec_hash < t_c >
{
    static constexpr size_t hash(size_t seed)
    {
        return seed * 33 ^ static_cast<unsigned>(t_c);
    }
};

// destination "template string" type
template < char... tt_c >
struct str_typed_string
{
    static void print()
    {
        rec_print < tt_c... > :: print();
        std::cout << std::endl;
    }

    static std::string get()
    {
        std::stringstream ss;
        rec_get <tt_c...> :: get(ss);
        return ss.str();
    }

    static constexpr size_t hash()
    {
        return rec_hash <tt_c...>::hash(5381);
    }
};

// struct to str_type a `char const*` to an `str_typed_string` type
template < typename T_StrProvider, unsigned t_len, char... tt_c >
struct str_type_impl
{
    using result = typename str_type_impl < T_StrProvider, t_len-1,
                                T_StrProvider::KEY()[t_len-1],
                                tt_c... > :: result;
};

template < typename T_StrProvider, char... tt_c >
struct str_type_impl < T_StrProvider, 0, tt_c... >
{
     using result = str_typed_string < tt_c... >;
};

// syntactical sugar
template < typename T_StrProvider >
using str_type = typename str_type_impl < T_StrProvider, c_strlen(T_StrProvider::KEY()) > :: result;

} // end namespace

namespace mc {

/*

template<typename ... Args>
class get_type
{
    template <std::size_t N>
    using type = typename std::tuple_element<N, std::tuple<Args...> >::type;
};

template <typename Function, typename T>
void __foreach_tuple(Function&& func, T&& elem)
{
    func(elem);
}

template <typename Function, typename T, typename ... Args>
void __foreach_tuple(Function&& func, T&& elem, Args&& ... args)
{
    // static_assert(std::is_same<T, get_type<Args>::type<0> >::value, "");
    func(elem);
    __foreach_tuple(std::forward<Function>(func), std::forward<Args>(args)...);
}

template <typename ... Args, typename Function, std::size_t... N>
void _foreach_tuple(const std::tuple<Args...>& t, Function&& func, std::index_sequence<N...>)
{
    __foreach_tuple(std::forward<Function>(func), std::get<N>(t)...);
}

template <typename ... Args, typename Function>
void foreach_tuple(const std::tuple<Args...>& t, Function&& func)
{
    _foreach_tuple(t, std::forward<Function>(func), std::make_index_sequence  <
                                                                std::tuple_size< std::tuple<Args...> >::value
                                                                            >());
}

template <typename Function, typename ... Args>
void foreach_args(Function&& func, Args&& ... args)
{
    foreach_tuple(std::make_tuple(std::forward<Args>(args)...), std::forward<Function>(func));
}

template <typename T, std::size_t... N>
auto _vector_to_tuple(const std::vector<T>& v, std::index_sequence<N...>)
{
	return std::make_tuple(v[N]...);
}

template <std::size_t N, typename T>
auto vector_to_tuple(const std::vector<T>& v)
{
	assert(v.size() >= N);
	return _vector_to_tuple(v, std::make_index_sequence<N>());
}

template <typename T, typename ... Args>
std::vector<T> tuple_to_vector(const std::tuple<T, Args...>& t)
{
    std::vector<T> v;
    foreach_tuple(t, [&v](const auto& d) {
        v.emplace_back(d);
    });
    return v;
}

// http://aherrmann.github.io/programming/2016/02/28/unpacking-tuples-in-cpp14/
template <class F, size_t... Is>
constexpr auto index_apply_impl(F&& f, std::index_sequence<Is...>) {
    return f(std::integral_constant<size_t, Is> {}...);
}

template <size_t N, class F>
constexpr auto index_apply(F&& f) {
    return index_apply_impl(std::forward<F>(f), std::make_index_sequence<N>{});
}

template <class Tuple>
constexpr auto _head(Tuple t) {
    return index_apply<1>([&](auto... Is) {
        return std::make_tuple(std::get<Is>(t)...);
    });
}

*/

template <class Tuple>
constexpr auto head(Tuple t)
{
    return std::get<0>(_head(t));
}

/*

template <class Tuple>
constexpr auto tail(Tuple t) {
    return index_apply<std::tuple_size<Tuple>{}-1u>([&](auto... Is) {
        return std::make_tuple(std::get<Is+1u>(t)...);
    });
}

template <class Tuple>
constexpr auto reverse(Tuple t) {
    return index_apply<std::tuple_size<Tuple>{}>(
        [&](auto... Is) {
            return std::make_tuple(
                std::get<std::tuple_size<Tuple>{} - 1 - Is>(t)...);
        });
}

// http://open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3829.pdf
template <typename Function, typename Tuple>
constexpr auto apply(Function&& f, Tuple&& t) {
    return index_apply<std::tuple_size<Tuple>{}>(
        [&](auto... Is) {
            return std::forward<Function>(f)( std::get<Is>(std::forward<Tuple>(t))... );
        });
}

*/

// http://talesofcpp.fusionfenix.com/post-14/true-story-moving-past-bind
template <typename F, typename ...Args>
auto bind(F&& f, Args&&... args) {
	return [
		f = std::forward<F>(f)
		, args = std::make_tuple(std::forward<Args>(args)...)
	]() mutable -> decltype(auto) {
		return apply(std::move(f), std::move(args));
	};
}

} // end namespace mc

// method macros
#define DEFINE_KEY(__CLASS__) \
	constexpr static char const* KEY() { return #__CLASS__; } \
	virtual const std::string& getKEY() const { static std::string key = #__CLASS__; return key; } \

// method non-macros (yes, exists optional macro :D)
#define DEFINE_HASH(__CLASS__)  \
	namespace std {             \
	template <>                 \
	struct hash<__CLASS__>      \
	{ size_t operator()() const { static size_t h = std::hash<std::string>()(#__CLASS__); return h; }	}; } \

#define DEFINE_HASH_CUSTOM(__CLASS__, __TYPE__, __VALUE__)  \
	namespace std {             \
	template <>                 \
	struct hash<__CLASS__>      \
	{ size_t operator()() const { static size_t h = std::hash<__TYPE__>()(__VALUE__); return h; }	}; } \

template<typename T>
class has_key
{
	typedef char(&yes)[2];

	template<typename> struct Exists;

	template<typename V>
	static yes CheckMember(Exists<decltype(&V::KEY)>*);
	template<typename>
	static char CheckMember(...);

public:
	static const bool value = (sizeof(CheckMember<T>(0)) == sizeof(yes));
};

template<typename T>
class has_instance
{
	typedef char(&yes)[2];

	template<typename> struct Exists;

	template<typename V>
	static yes CheckMember(Exists<decltype(&V::instance)>*);
	template<typename>
	static char CheckMember(...);

public:
	static const bool value = (sizeof(CheckMember<T>(0)) == sizeof(yes));
};

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

