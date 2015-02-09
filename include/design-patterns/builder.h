#ifndef _BUILDER_H_
#define _BUILDER_H_

#include "common.h"

template <typename T, typename... Args>
class Builder
{
public:
	DEFINE_KEY(Builder<T>)

	typedef std::string Key;
	typedef typename std::function<std::shared_ptr<T> (Args&&...)> Value;

	static typename T::Builder& instance()
	{
		static typename T::Builder builder;
		return builder;
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

	bool exists(const Key& key_impl, Args&&... data)
	{
		Key key = T::get_data_key(std::forward<Args>(data)...);
		return (_map_cache.find(key) != _map_cache.end());
	}

	std::shared_ptr<T> get(const Key& key_impl, Args&&... data)
	{
		Key key = T::get_data_key(std::forward<Args>(data)...);

		auto it = _map_cache.find(key);
		if (it != _map_cache.end())
		{
			return it->second.lock();
		}
		else
		{
			auto itc =  _map_creators.find(key_impl);
			if(itc == _map_creators.end())
			{
				std::cout << "Can't found key in map: " << key << std::endl;
				throw std::exception();
			}
		
			std::shared_ptr<T> new_product = (itc->second)(std::forward<Args>(data)...);
			_map_cache.insert(std::make_pair(key, std::weak_ptr<T>(new_product)));
			return new_product;
		}
	}

	template <typename U>
	std::shared_ptr<U> get(Args&&... data)
	{
		return std::dynamic_pointer_cast<U>(get(get_key<U>(), std::forward<Args>(data)...));
	}
private:
	std::map<Key, Value> _map_creators;
	std::map<Key, std::weak_ptr<T> > _map_cache;
};

template<typename T, typename U, typename... Args>
class BuilderRegistrator
{
public:
	explicit BuilderRegistrator()
	{
		indirection(make_int_sequence< sizeof...(Args) >{});
	}

	static std::shared_ptr<T> get(Args&&... data)
	{
		return std::make_shared<U>(data...);
	}

protected:
	template <int... Is>
	void indirection(int_sequence<Is...>)
	{
		T::Builder::instance().template register_type<U>(std::bind(&BuilderRegistrator<T, U, Args&&...>::get, placeholder_template < Is > {}...));
	}
};

#endif
