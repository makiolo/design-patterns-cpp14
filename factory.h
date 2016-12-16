// design-patterns-cpp14 by Ricardo Marmolejo García is licensed under a
// Creative Commons Reconocimiento 4.0 Internacional License.
// http://creativecommons.org/licenses/by/4.0/
//
#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <iostream>
#include "metacommon/common.h"

namespace dp14 {

template <typename T, typename U, typename... Args>
class factory_registrator;

template <typename T, typename... Args>
class factory
{
public:
	using key_impl = size_t;
	using registrator_function = std::function<std::shared_ptr<T>(Args...)>;
	using registrator_container = std::unordered_map<key_impl, registrator_function>;
	template <typename U>
	using registrator = factory_registrator<T, U, Args...>;

	template <typename U>					
	typename std::enable_if<(has_key<U>::value), key_impl>::type get_key() const
	{
		// return ctti::str_type<U>::hash();
		return std::hash<std::string>()(ctti::str_type<U>()::get());
	}

	template <typename U>
	typename std::enable_if<(!has_key<U>::value), key_impl>::type get_key() const
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

	std::shared_ptr<T> create(const std::string& key_impl_str, Args&&... data) const
	{
		key_impl keyimpl = std::hash<std::string>()(key_impl_str);
		return create(keyimpl, std::forward<Args>(data)...);
	}

	template <typename U>
	std::shared_ptr<U> create(Args&&... data) const
	{
		return std::dynamic_pointer_cast<U>(create(get_key<U>(), std::forward<Args>(data)...));
	}

	static typename T::factory& instance()
	{
		static typename T::factory factory;
		return factory;
	}

protected:
	std::shared_ptr<T> create(const key_impl& keyimpl, Args&&... data) const
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

	static std::shared_ptr<T> create(Args&&... data)
	{
		return std::make_shared<U>(std::forward<Args>(data)...);
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

}

#endif
