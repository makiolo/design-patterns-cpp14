#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "common.h"

template <typename T, typename... Args>
class Factory
{
public:
	DEFINE_KEY(Factory<T>)

	using Key = std::string;
	using Value = typedef typename std::function<std::shared_ptr<T> (Args&&...)>;

	static typename T::Factory& instance()
	{
		static typename T::Factory factory;
		return factory;
	}

	template <typename U>
	Key get_key()
	{
		return U::KEY();
	}

	template <typename U>
	void register_type(const Value& value)
	{
		_map_creators[get_key<U>()] = value;
	}

	std::shared_ptr<T> create(const Key& key_impl, Args&&... data)
	{
		auto it =  _map_creators.find(key_impl);
		if(it == _map_creators.end())
		{
			std::cout << "Can't found key in map: " << key_impl << std::endl;
			throw std::exception();
		}
		return (it->second)(std::forward<Args>(data)...);
	}

	template <typename U>
	std::shared_ptr<U> create(Args&&... data)
	{
		return std::dynamic_pointer_cast<U>(create(get_key<U>(), std::forward<Args>(data)...));
	}
private:
	std::map<Key, Value> _map_creators;
};

template<typename T, typename U, typename... Args>
class FactoryRegistrator
{
public:
	explicit FactoryRegistrator()
	{
		indirection(make_int_sequence< sizeof...(Args) >{});
	}

	explicit FactoryRegistrator(Factory<T, Args...>& factory)
	{
		indirection(factory, make_int_sequence< sizeof...(Args) >{});
	}

	static std::shared_ptr<T> create(Args&&... data)
	{
		return std::make_shared<U>(data...);
	}

protected:
	template <int... Is>
	void indirection(int_sequence<Is...>)
	{
		T::Factory::instance().template register_type<U>(std::bind(&FactoryRegistrator<T, U, Args&&...>::create, placeholder_template < Is > {}...));
	}

	template <int... Is>
	void indirection(Factory<T, Args...>& factory, int_sequence<Is...>)
	{
		factory.template register_type<U>(std::bind(&FactoryRegistrator<T, U, Args&&...>::create, placeholder_template < Is > {}...));
	}
};

#endif
