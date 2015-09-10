// design-patterns-cpp14 by Ricardo Marmolejo García is licensed under a Creative Commons Reconocimiento 4.0 Internacional License.
// http://creativecommons.org/licenses/by/4.0/
//
#ifndef _MEMOIZE_H_
#define _MEMOIZE_H_

#include "common.h"

namespace dp14 {

template<typename T, typename U, typename... Args>
class memoize_registrator;

template <typename T, typename... Args>
class memoize
{
public:
	using KeyCache = size_t;
	using KeyImpl = size_t;
	using RegistratorFunction = std::function<std::shared_ptr<T> (Args...)>;
	using cache_iterator = typename std::unordered_map<KeyCache, std::weak_ptr<T> >::const_iterator;
	template<typename U> using registrator = memoize_registrator<T, U, Args...>;

	static typename T::memoize& instance()
	{
		static typename T::memoize memoize;
		return memoize;
	}

	template <typename U>
	inline KeyImpl get_impl_hash() const
	{
		return std::hash<U>()();
	}

	inline KeyCache get_base_hash(const KeyImpl& key_impl, Args&&... data) const
	{
		return key_impl ^ dp14::hash<T>()(std::forward<Args>(data)...);
	}

	template <typename U, typename F>
	void register_type(F&& value)
	{
		_map_registrators[get_impl_hash<U>()] = std::forward<F>(value);
	}

	inline bool exists(const std::string& key_impl_str, Args&&... data) const
	{
		KeyImpl key_impl = std::hash<std::string>()(key_impl_str);
		return exists(key_impl, std::forward<Args>(data)...);
	}

	template <typename U>
	inline bool exists(Args&&... data) const
	{
		return exists(get_impl_hash<U>(), std::forward<Args>(data)...);
	}

	std::shared_ptr<T> get(const std::string& key_impl_str, Args&&... data) const
	{
		KeyImpl key_impl = std::hash<std::string>()(key_impl_str);
		KeyCache key = get_base_hash(key_impl, std::forward<Args>(data)...);
		return get(key_impl, key, std::forward<Args>(data)...);
	}

	template <typename U>
	inline std::shared_ptr<U> get(Args&&... data) const
	{
		return std::dynamic_pointer_cast<U>(get(get_impl_hash<U>(), std::forward<Args>(data)...));
	}

protected:
	std::shared_ptr<T> get(const KeyImpl& key_impl, KeyCache key, Args&&... data) const
	{
		cache_iterator it = _exists(key_impl, std::forward<Args>(data)...);
		if(it != _map_cache.end())
		{
			return it->second.lock();
		}

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

	std::shared_ptr<T> get(const KeyImpl& key_impl, Args&&... data) const
	{
		KeyCache key = get_base_hash(key_impl, std::forward<Args>(data)...);
		return get(key_impl, key, std::forward<Args>(data)...);
	}

	bool exists(const KeyImpl& key_impl, Args&&... data) const
	{
		return _exists(key_impl, std::forward<Args>(data)...) != _map_cache.end();
	}

	cache_iterator _exists(const KeyImpl& key_impl, Args&&... data) const
	{
		KeyCache key = get_base_hash(key_impl, std::forward<Args>(data)...);
		cache_iterator it = _map_cache.find(key);
		cache_iterator ite = _map_cache.end();
		if (it != _map_cache.end())
		{
			// pointer cached can be dangled
			if(!it->second.expired())
			{
				return it;
			}
		}
		return ite;
	}
protected:
	std::unordered_map<KeyImpl, RegistratorFunction> _map_registrators;
	mutable std::unordered_map<KeyCache, std::weak_ptr<T> > _map_cache;
};

template<typename T, typename U, typename ... Args>
class memoize_registrator
{
public:
	explicit memoize_registrator()
	{
		register_to_singleton(make_int_sequence< sizeof...(Args) >{});
	}

	explicit memoize_registrator(memoize<T, Args...>& memoize)
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
		T::memoize::instance().template register_type<U>(std::bind(&memoize_registrator<T, U, Args...>::get, placeholder_template < Is > {}...));
	}

	template <int... Is>
	void register_in_a_memoize(memoize<T, Args...>& memoize, int_sequence<Is...>)
	{
		memoize.template register_type<U>(std::bind(&memoize_registrator<T, U, Args...>::get, placeholder_template < Is > {}...));
	}
};

}

#endif

