// design-patterns-cpp14 by Ricardo Marmolejo García is licensed under a
// Creative Commons Reconocimiento 4.0 Internacional License.
// http://creativecommons.org/licenses/by/4.0/
//
#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <iostream>
#include "common.h"

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

	static typename T::factory& instance()
	{
		static typename T::factory factory;
		return factory;
	}

	template <typename U,
				class = typename std::enable_if<
					(has_key<U>::value)
				>::type
			>
	key_impl get_key(int=0) const
	{
		return std::hash<std::string>()(U::KEY());
	}

	template <typename U,
				class = typename std::enable_if<
					(!has_key<U>::value)
				>::type
			>
	key_impl get_key(long=0) const
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
			_map_registrators[keyimpl] = std::forward<F>(value);
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
	explicit factory_registrator()
		: _f(nullptr)
	{
		register_to_singleton(make_int_sequence<sizeof...(Args)>{});
		std::cout << "register implementation (singleton)" << std::endl;
	}

	explicit factory_registrator(factory<T, Args...>& f)
		: _f(nullptr)
	{
		register_in_a_factory(f, make_int_sequence<sizeof...(Args)>{});
		std::cout << "register implementation" << std::endl;
	}

	static std::shared_ptr<T> create(Args&&... data)
	{
		return std::make_shared<U>(std::forward<Args>(data)...);
	}
	
	~factory_registrator()
	{
		if(_f != nullptr)
		{
			std::cout << "unregistering implementation" << std::endl;
			_f->template unregister_type<U>();
		}
		else
		{
			std::cout << "unregistering implementation (singleton)" << std::endl;
			T::factory::instance().template unregister_type<U>();
		}
	}

protected:
	template <int... Is>
	void register_to_singleton(int_sequence<Is...>)
	{
		T::factory::instance().template register_type<U>(
			std::bind(&factory_registrator<T, U, Args...>::create, placeholder_template<Is>{}...));
	}

	template <int... Is>
	void register_in_a_factory(factory<T, Args...>& f, int_sequence<Is...>)
	{
		// life factory is always greater (TODO: use weak_ptr)
		_f = &f;
		f.template register_type<U>(
			std::bind(&factory_registrator<T, U, Args...>::create, placeholder_template<Is>{}...));
	}
protected:
	factory<T, Args...>* _f;
};

}

#endif
