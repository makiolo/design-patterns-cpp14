// design-patterns-cpp14 by Ricardo Marmolejo García is licensed under a Creative Commons Reconocimiento 4.0 Internacional License.
// http://creativecommons.org/licenses/by/4.0/
//
#ifndef _MEMOIZE_H_
#define _MEMOIZE_H_

#include "common.h"

namespace dp14 {

template<typename T, typename U, typename... Args>
class MemoizeRegistrator;

template <typename T, typename... Args>
class Memoize
{
public:
	DEFINE_KEY(Memoize<T>)
	
	using Key = std::string;
	using Value = std::function<std::shared_ptr<T> (Args...)>;
	template<typename U> using Registrator = MemoizeRegistrator<T, U, Args...>;

	static typename T::Memoize& instance()
	{
		static typename T::Memoize memoize;
		return memoize;
	}

	template <typename U>
	Key get_key()
	{
		return U::KEY();
	}
	
	template <typename U, typename F>
	void register_type(F && value)
	{
		_map_creators[get_key<U>()] = std::forward<F>(value);
	}

	bool exists(const Key& key_impl, Args&&... data)
	{
		Key key = T::get_data_key(key_impl, std::forward<Args>(data)...);
		return (_map_cache.find(key) != _map_cache.end());
	}

	template <typename U>
	bool exists(Args&&... data)
	{
		return exists(get_key<U>(), std::forward<Args>(data)...);
	}

	std::shared_ptr<T> get(const Key& key_impl, Args&&... data)
	{
		Key key = T::get_data_key(key_impl, std::forward<Args>(data)...);

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

template<typename T, typename U, typename ... Args>
class MemoizeRegistrator
{
public:
	explicit MemoizeRegistrator()
	{
		register_to_singleton(make_int_sequence< sizeof...(Args) >{});
	}
	
	explicit MemoizeRegistrator(Memoize<T, Args...>& memoize)
	{
		register_in_a_memoize(memoize, make_int_sequence< sizeof...(Args) >{});
	}
	
	static std::shared_ptr<T> get(Args&&... data)
	{
		return std::make_shared<U>(std::forward<Args>(data)...);
	}
	
protected:
	template <int... Is>
	void register_to_singleton(int_sequence<Is...>)
	{
		T::Memoize::instance().template register_type<U>(std::bind(&MemoizeRegistrator<T, U, Args...>::get, placeholder_template < Is > {}...));
	}

	template <int... Is>
	void register_in_a_memoize(Memoize<T, Args...>& memoize, int_sequence<Is...>)
	{
		memoize.template register_type<U>(std::bind(&MemoizeRegistrator<T, U, Args...>::get, placeholder_template < Is > {}...));
	}
};

}

#endif

