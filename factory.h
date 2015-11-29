// design-patterns-cpp14 by Ricardo Marmolejo García is licensed under a
// Creative Commons Reconocimiento 4.0 Internacional License.
// http://creativecommons.org/licenses/by/4.0/
//
#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "common.h"

namespace dp14
{

template <typename T, typename U, typename... Args>
class factory_registrator;

template <typename T, typename... Args>
class factory
{
public:
	using KeyImpl = size_t;
	using RegistratorFunction = std::function<std::shared_ptr<T>(Args...)>;
	using registrator_container = std::unordered_map<KeyImpl, RegistratorFunction>;
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
	KeyImpl get_key(int=0) const
	{
		return std::hash<std::string>()(U::KEY());
	}

	template <typename U,
				class = typename std::enable_if<
					(!has_key<U>::value)
				>::type
			>
	KeyImpl get_key(long=0) const
	{
		return std::hash<U>()();
	}

	template <typename U, typename F>
	void register_type(F&& value)
	{
		_map_registrators[get_key<U>()] = std::forward<F>(value);
	}

	std::shared_ptr<T> create(const std::string& key_impl_str, Args&&... data) const
	{
		KeyImpl key_impl = std::hash<std::string>()(key_impl_str);
		return create(key_impl, std::forward<Args>(data)...);
	}

	template <typename U>
	std::shared_ptr<U> create(Args&&... data) const
	{
		return std::dynamic_pointer_cast<U>(create(get_key<U>(), std::forward<Args>(data)...));
	}

protected:
	std::shared_ptr<T> create(const KeyImpl& key_impl, Args&&... data) const
	{
		auto it = _map_registrators.find(key_impl);
		if (it == _map_registrators.end())
		{
			std::cout << "Can't found key in map: " << key_impl << std::endl;
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
	{
		register_to_singleton(make_int_sequence<sizeof...(Args)>{});
	}

	explicit factory_registrator(factory<T, Args...>& f)
	{
		register_in_a_factory(f, make_int_sequence<sizeof...(Args)>{});
	}

	static std::shared_ptr<T> create(Args&&... data)
	{
		return std::make_shared<U>(std::forward<Args>(data)...);
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
		f.template register_type<U>(
			std::bind(&factory_registrator<T, U, Args...>::create, placeholder_template<Is>{}...));
	}
};

}

#endif
