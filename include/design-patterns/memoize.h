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
	using KeyCache = size_t;
	using KeyImpl = size_t;
	using RegistratorFunction = std::function<std::shared_ptr<T> (Args...)>;
	template<typename U> using Registrator = MemoizeRegistrator<T, U, Args...>;

	static typename T::Memoize& instance()
	{
		static typename T::Memoize memoize;
		return memoize;
	}

	template <typename U>
	inline KeyImpl get_impl_hash()
	{
		return std::hash<U>()();
	}

	inline KeyCache get_base_hash(const KeyImpl& key_impl, Args&&... data)
	{
		return key_impl ^ dp14::hash<T>()(std::forward<Args>(data)...);
	}

	template <typename U, typename F>
	void register_type(F&& value)
	{
		_map_registrators[get_impl_hash<U>()] = std::forward<F>(value);
	}

	inline bool exists(const std::string& key_impl_str, Args&&... data)
	{
		KeyImpl key_impl = std::hash<std::string>()(key_impl_str);
		return exists(key_impl, std::forward<Args>(data)...);
	}

	template <typename U>
	inline bool exists(Args&&... data)
	{
		return exists(get_impl_hash<U>(), std::forward<Args>(data)...);
	}

	std::shared_ptr<T> get(const std::string& key_impl_str, Args&&... data)
	{
		KeyImpl key_impl = std::hash<std::string>()(key_impl_str);
		KeyCache key = get_base_hash(key_impl, std::forward<Args>(data)...);
		return get(key_impl, key, std::forward<Args>(data)...);
	}

	template <typename U>
	inline std::shared_ptr<U> get(Args&&... data)
	{
		return std::dynamic_pointer_cast<U>(get(get_impl_hash<U>(), std::forward<Args>(data)...));
	}

protected:
	std::shared_ptr<T> get(const KeyImpl& key_impl, KeyCache key, Args&&... data)
	{
		auto it = _map_cache.find(key);
		if (it != _map_cache.end())
		{
			if(!it->second.expired())
			{
				return it->second.lock();
			}
		}

		// create element
		auto itc =  _map_registrators.find(key_impl);
		if(itc == _map_registrators.end())
		{
			std::cout << "Can't found key in map: " << key << std::endl;
			throw std::exception();
		}

		std::shared_ptr<T> new_product = (itc->second)(std::forward<Args>(data)...);
		_map_cache[key] = std::weak_ptr<T>(new_product);
		return new_product;
	}

	std::shared_ptr<T> get(const KeyImpl& key_impl, Args&&... data)
	{
		KeyCache key = get_base_hash(key_impl, std::forward<Args>(data)...);
		return get(key_impl, key, std::forward<Args>(data)...);
	}

	bool exists(const KeyImpl& key_impl, Args&&... data)
	{
		KeyCache key = get_base_hash(key_impl, std::forward<Args>(data)...);
		auto it = _map_cache.find(key);
		if (it != _map_cache.end())
		{
			if(!it->second.expired())
			{
				return true;
			}
		}
		return false;
	}
protected:
	std::unordered_map<KeyImpl, RegistratorFunction> _map_registrators;
	std::unordered_map<KeyCache, std::weak_ptr<T> > _map_cache;
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

