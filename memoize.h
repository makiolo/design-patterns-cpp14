#ifndef _MEMOIZE_H_
#define _MEMOIZE_H_

#include "metacommon/common.h"

namespace dp14 {

template <typename T, typename U, typename... Args>
class memoize_registrator;

template <typename T, typename... Args>
class memoize
{
public:
	using key_cache = size_t;
	using key_impl = size_t;
	using registrator_function = std::function<std::shared_ptr<T>(Args...)>;
	using cache_container = std::unordered_map<key_cache, std::weak_ptr<T>>;
	using registrator_container = std::unordered_map<key_impl, registrator_function>;
	using cache_iterator = typename cache_container::const_iterator;
	template <typename U>
	using registrator = memoize_registrator<T, U, Args...>;

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

	inline key_cache get_base_hash(const key_impl& keyimpl, Args&&... data) const
	{
		return keyimpl ^ dp14::hash<T>()(std::forward<Args>(data)...);
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

	inline bool exists(const std::string& keyimpl_str, Args&&... data) const
	{
		key_impl keyimpl = std::hash<std::string>()(keyimpl_str);
		return exists(keyimpl, std::forward<Args>(data)...);
	}

	template <typename U>
	inline bool exists(Args&&... data) const
	{
		return exists(get_key<U>(), std::forward<Args>(data)...);
	}

	std::shared_ptr<T> get(const std::string& keyimpl_str, Args&&... data) const
	{
		key_impl keyimpl = std::hash<std::string>()(keyimpl_str);
		key_cache key = get_base_hash(keyimpl, std::forward<Args>(data)...);
		return get(keyimpl, key, std::forward<Args>(data)...);
	}

	template <typename U>
	inline std::shared_ptr<U> get(Args&&... data) const
	{
		return std::dynamic_pointer_cast<U>(get(get_key<U>(), std::forward<Args>(data)...));
	}

	static typename T::memoize& instance()
	{
		static typename T::memoize memoize;
		return memoize;
	}

protected:
	std::shared_ptr<T> get(const key_impl& keyimpl, key_cache key, Args&&... data) const
	{
		cache_iterator it = _exists(keyimpl, std::forward<Args>(data)...);
		if (it != _map_cache.end())
		{
			return it->second.lock();
		}

		auto itc = _map_registrators.find(keyimpl);
		if (itc == _map_registrators.end())
		{
			std::cout << "Can't found key in map: " << key << std::endl;
			throw std::exception();
		}

		std::shared_ptr<T> new_product = (itc->second)(std::forward<Args>(data)...);
		_map_cache.emplace(key, std::weak_ptr<T>(new_product));
		return new_product;
	}

	std::shared_ptr<T> get(const key_impl& keyimpl, Args&&... data) const
	{
		key_cache key = get_base_hash(keyimpl, std::forward<Args>(data)...);
		return get(keyimpl, key, std::forward<Args>(data)...);
	}

	bool exists(const key_impl& keyimpl, Args&&... data) const
	{
		return _exists(keyimpl, std::forward<Args>(data)...) != _map_cache.end();
	}

	cache_iterator _exists(const key_impl& keyimpl, Args&&... data) const
	{
		key_cache key = get_base_hash(keyimpl, std::forward<Args>(data)...);
		cache_iterator it = _map_cache.find(key);
		cache_iterator ite = _map_cache.end();
		if (it != ite)
		{
			// pointer cached can be dangled
			if (!it->second.expired())
			{
				return it;
			}
			else
			{
				// remove expired dangled pointer
				_map_cache.erase(key);
			}
		}
		return ite;
	}

protected:
	registrator_container _map_registrators;
	mutable cache_container _map_cache;
};

template <typename T, typename U, typename... Args>
class memoize_registrator
{
public:
	template <class = typename std::enable_if<
					(has_instance<typename T::memoize>::value)
				>::type>
	explicit memoize_registrator()
		: _m(T::memoize::instance())
	{
		_register(make_int_sequence<sizeof...(Args)>{});
	}

	explicit memoize_registrator(memoize<T, Args...>& m)
		: _m(m)
	{
		_register(make_int_sequence<sizeof...(Args)>{});
	}

	static std::shared_ptr<T> get(Args&&... data)
	{
		return std::make_shared<U>(std::forward<Args>(data)...);
	}

	~memoize_registrator()
	{
		_m.template unregister_type<U>();
	}

protected:
	template <int... Is>
	void _register(int_sequence<Is...>)
	{
		_m.template register_type<U>(
			std::bind(&memoize_registrator<T, U, Args...>::get, placeholder_template<Is>{}...));
	}
protected:
	memoize<T, Args...>& _m;
};

}

#endif
