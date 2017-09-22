#ifndef _MEMOIZE_H_
#define _MEMOIZE_H_

#include <metacommon/common.h>

namespace dp14 {

template <typename T, typename U, typename... Args>
class memoize_registrator;

template <typename T, typename... Args>
class memoize;

namespace detail {
	namespace memoize {

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
class memoize
{
public:
	using key_cache = size_t;
	using key_impl = size_t;
	using registrator_function = std::function<std::shared_ptr<T>(Args...)>;
	using cache_container = std::unordered_map<key_cache, std::weak_ptr<T>>;
	using cache_shared_container = std::unordered_map<key_cache, std::shared_ptr<T>>;
	using registrator_container = std::unordered_map<key_impl, registrator_function>;
	using cache_iterator = typename cache_container::const_iterator;
	template <typename U>
	using registrator = memoize_registrator<T, U, Args...>;
	template <typename U>
	using reg = memoize_registrator<T, U, Args...>;

	template <typename U>					
	static typename std::enable_if<(has_key<U>::value), key_impl>::type get_key()
	{
		return detail::memoize::get_hash(ctti::str_type<U>::get());
	}

	template <typename U>
	static typename std::enable_if<(!has_key<U>::value), key_impl>::type get_key()
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

	template <typename TYPE_KEY>
	inline bool exists(TYPE_KEY keyimpl_str, Args&&... data) const
	{
		auto keyimpl = detail::memoize::get_hash(keyimpl_str);
		return exists(keyimpl, std::forward<Args>(data)...);
	}

	template <typename U>
	inline bool exists(Args&&... data) const
	{
		return exists(get_key<U>(), std::forward<Args>(data)...);
	}

	template <typename TYPE_KEY>
	std::shared_ptr<T> get(TYPE_KEY keyimpl_str, Args&&... data) const
	{
		auto keyimpl = detail::memoize::get_hash(keyimpl_str);
		key_cache key = get_base_hash(keyimpl, std::forward<Args>(data)...);
		auto obj = _get(keyimpl, key, std::forward<Args>(data)...);
		_map_cache_shared.emplace(key, obj);
		return obj;
	}

	template <typename TYPE_KEY>
	auto execute(TYPE_KEY keyimpl_str, Args&&... data) const
	{
		auto keyimpl = detail::memoize::get_hash(keyimpl_str);
		key_cache key = get_base_hash(keyimpl, std::forward<Args>(data)...);
		auto code = _get(keyimpl, key, std::forward<Args>(data)...);
		_map_cache_shared.emplace(key, code);
		return code->get();
	}
	
	template <typename TYPE_KEY>
	void clear(TYPE_KEY keyimpl_str, Args&&... data) const
	{
		auto keyimpl = detail::memoize::get_hash(keyimpl_str);
		key_cache key = get_base_hash(keyimpl, std::forward<Args>(data)...);
		_map_cache_shared.erase(key);
	}
	
	void clear() const
	{
		_map_cache_shared.clear();
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
	std::shared_ptr<T> _get(const key_impl& keyimpl, key_cache key, Args&&... data) const
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
		return _get(keyimpl, key, std::forward<Args>(data)...);
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
	mutable cache_shared_container _map_cache_shared;
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

template <typename Result, typename ... Args>
struct code_once
{
	using memoize = dp14::memoize<code_once, Args...>;
	virtual ~code_once() { ; }
	void set(Result r) {_r = std::move(r);}
	const Result& get() const {return _r;}
protected:
	Result _r;
};

template <typename Result, typename ... Args>
using repository_once = typename dp14::code_once<Result, Args...>::memoize;

}

#endif
