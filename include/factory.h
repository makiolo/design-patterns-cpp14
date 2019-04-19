// design-patterns-cpp14 by Ricardo Marmolejo García is licensed under a
// Creative Commons Reconocimiento 4.0 Internacional License.
// http://creativecommons.org/licenses/by/4.0/
//
#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <iostream>
#include <metacommon/common.h>

namespace dp14 {

template <typename T, typename U, typename... Args>
class factory_registrator;

template <typename T, typename... Args>
class factory;

namespace detail {
	namespace factory {
		template <typename TYPE_KEY>
		size_t get_hash(TYPE_KEY key_impl_str)
		{
			return std::hash<TYPE_KEY>()(key_impl_str);
		}

		template <>
		size_t get_hash<const char*>(const char* key_impl_str)
		{
			return std::hash<std::string>()(std::string(key_impl_str));
		}
	}
}

template <typename T, typename... Args>
class factory
{
public:
	using key_impl = size_t;
	using registrator_function = std::function<std::unique_ptr<T>(Args...)>;
	using registrator_container = std::unordered_map<key_impl, registrator_function>;
	template <typename U>
	using registrator = factory_registrator<T, U, Args...>;
	template <typename U>
	using reg = factory_registrator<T, U, Args...>;

	template <typename U>
	static typename std::enable_if<(has_key<U>::value), key_impl>::type get_key()
	{
		return detail::factory::get_hash(ctti::str_type<U>::get());
	}

	template <typename U>
	static typename std::enable_if<(!has_key<U>::value), key_impl>::type get_key()
	{
		return std::hash<U>()();
	}

	template <typename U, typename F>
	void register_type(F&& value)
	{
		key_impl keyimpl = get_key<U>();
		auto it = _map_registrators.find(keyimpl);
		if (it != _map_registrators.end())
		{
			std::cout << "Already registered key " << keyimpl << std::endl;
			throw std::exception();
		}
		else
		{
			_map_registrators.emplace(keyimpl, std::forward<F>(value));
		}
	}

	template <typename U>
	void unregister_type()
	{
		key_impl keyimpl = get_key<U>();
		auto it = _map_registrators.find(keyimpl);
		if (it != _map_registrators.end())
		{
			_map_registrators.erase(get_key<U>());
		}
		else
		{
			std::cout << "Already unregistered key " << keyimpl << std::endl;
			throw std::exception();
		}
	}
	
	template <typename TYPE_KEY>
	std::unique_ptr<T> create(TYPE_KEY key_impl_str, Args&&... data) const
	{
		auto keyimpl = detail::factory::get_hash(key_impl_str);
		return _create(keyimpl, std::forward<Args>(data)...);
	}
	
	template <typename TYPE_KEY>
	auto execute(TYPE_KEY keyimpl_str, Args&&... data) const
	{
		auto keyimpl = detail::factory::get_hash(keyimpl_str);
		auto code = _create(keyimpl, std::forward<Args>(data)...);
		return code->get();
	}

	static typename T::factory& instance()
	{
		static typename T::factory factory;
		return factory;
	}

protected:
	std::unique_ptr<T> _create(const key_impl& keyimpl, Args&&... data) const
	{
		auto it = _map_registrators.find(keyimpl);
		if (it == _map_registrators.end())
		{
			std::cout << "Can't found key in map: " << keyimpl << std::endl;
			throw std::exception();
		}
		return (it->second)(std::forward<Args>(data)...);
	}

protected:
	registrator_container _map_registrators;
};

template <typename T, typename U, typename... Args>
class factory_registrator
{
public:
	template <class = typename std::enable_if<
					(has_instance<typename T::factory>::value)
				>::type>
	explicit factory_registrator()
		: _f(T::factory::instance())
	{
		_register(make_int_sequence<sizeof...(Args)>{});
	}

	explicit factory_registrator(factory<T, Args...>& f)
		: _f(f)
	{
		_register(make_int_sequence<sizeof...(Args)>{});
	}

	static std::unique_ptr<T> create(Args&&... data)
	{
		return std::make_unique<U>(std::forward<Args>(data)...);
	}

	~factory_registrator()
	{
		_f.template unregister_type<U>();
	}

protected:
	template <int... Is>
	void _register(int_sequence<Is...>)
	{
		_f.template register_type<U>(
			std::bind(&factory_registrator<T, U, Args...>::create, placeholder_template<Is>{}...));
	}
protected:
	factory<T, Args...>& _f;
};

template <typename Result, typename ... Args>
struct code
{
	using factory = dp14::factory<code, Args...>;
	virtual ~code() { ; }
	void set(Result r) {_r = std::move(r);}
	const Result& get() const {return _r;}
protected:
	Result _r;
};

template <typename Result, typename ... Args>
using repository = typename dp14::code<Result, Args...>::factory;

}

#endif
