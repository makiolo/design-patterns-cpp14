// design-patterns-cpp14 by Ricardo Marmolejo García is licensed under a Creative Commons Reconocimiento 4.0 Internacional License.
// http://creativecommons.org/licenses/by/4.0/
//
#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "common.h"

namespace dp14 {

template<typename T, typename U, typename ... Args>
class FactoryRegistrator;

template <typename T, typename ... Args>
class Factory
{
public:
	using KeyImpl = size_t;
	using RegistratorFunction = std::function<std::shared_ptr<T> (Args...)>;
	template<typename U> using Registrator = FactoryRegistrator<T, U, Args...>;

	static typename T::Factory& instance()
	{
		static typename T::Factory factory;
		return factory;
	}

	template <typename U>
	KeyImpl get_key()
	{
		return std::hash<U>()();
	}

	template <typename U, typename F>
	void register_type(F&& value)
	{
		_map_registrators[get_key<U>()] = std::forward<F>(value);
	}

	std::shared_ptr<T> create(const std::string& key_impl_str, Args&&... data)
	{
		KeyImpl key_impl = std::hash<std::string>()(key_impl_str);
		return create(key_impl, std::forward<Args>(data)...);
	}

	template <typename U>
	std::shared_ptr<U> create(Args&&... data)
	{
		return std::dynamic_pointer_cast<U>(create(get_key<U>(), std::forward<Args>(data)...));
	}

protected:
	std::shared_ptr<T> create(const KeyImpl& key_impl, Args&&... data)
	{
		auto it =  _map_registrators.find(key_impl);
		if(it == _map_registrators.end())
		{
			std::cout << "Can't found key in map: " << key_impl << std::endl;
			throw std::exception();
		}
		return (it->second)(std::forward<Args>(data)...);
	}
protected:
	std::map<KeyImpl, RegistratorFunction> _map_registrators;
};

template<typename T, typename U, typename ... Args>
class FactoryRegistrator
{
public:
	explicit FactoryRegistrator()
	{
		register_to_singleton(make_int_sequence< sizeof...(Args) >{});
	}

	explicit FactoryRegistrator(Factory<T, Args...>& factory)
	{
		register_in_a_factory(factory, make_int_sequence< sizeof...(Args) >{});
	}

	static std::shared_ptr<T> create(Args&&... data)
	{
		return std::make_shared<U>(std::forward<Args>(data)...);
	}

protected:
	template <int... Is>
	void register_to_singleton(int_sequence<Is...>)
	{
		T::Factory::instance().template register_type<U>(std::bind(&FactoryRegistrator<T, U, Args...>::create, placeholder_template<Is>{}...));
	}

	template <int... Is>
	void register_in_a_factory(Factory<T, Args...>& factory, int_sequence<Is...>)
	{
		factory.template register_type<U>(std::bind(&FactoryRegistrator<T, U, Args...>::create, placeholder_template<Is>{}...));
	}
};

}

#endif
