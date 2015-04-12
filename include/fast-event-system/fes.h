#ifndef _FAST_EVENT_SYSTEM_
#define _FAST_EVENT_SYSTEM_

#include <functional>
#include <string>
#include <memory>
#include <map>
#include <exception>
#include <thread>
#include <chrono>
#include <deque>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <design-patterns/common.h>

#ifndef _MSC_VER
namespace std
{
	template <bool B, typename T = void> using enable_if_t = typename std::enable_if<B, T>::type;

	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique(Args&&... args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
}
#endif

namespace fes {

template <typename ... Args>
class method;

template <typename ... Args>
class connection
{
public:
	using vector_methods = std::vector<method<Args...> >;
	using connection_id = typename vector_methods::iterator;

	connection(vector_methods& registered, connection_id it)
		: _registered(registered)
		, _it(it)
	{
		
	}
	connection(const connection<Args...>&) = default;
	connection<Args...> operator=(const connection<Args...>& other) = delete;

	~connection()
	{

	}

	void disconnect()
	{
		_registered.erase(_it);
	}
	
protected:
	vector_methods& _registered;
	connection_id _it;
};


template <typename ... Args>
class connection_scoped
{
public:
	connection_scoped()
	{
		
	}

	connection_scoped(const connection<Args ...>& other)
	{
		_connection = std::make_unique<connection<Args...>>(other);
	}

	connection_scoped(connection_scoped<Args...>&& other)
		: _connection(std::move(other._connection))
	{
		
	}

	~connection_scoped()
	{
		if (_connection)
		{
			_connection->disconnect();
		}
	}
	
	const connection_scoped<Args...>& operator=(const connection<Args ...>&other)
	{
		_connection = std::make_unique<connection<Args...>>(other);
		return *this;
	}
	
protected:
	std::unique_ptr<connection<Args...>> _connection;
};

template <typename ... Args>
class connection_shared
{
public:
	connection_shared()
	{
		
	}
	
	void disconnect()
	{
		if (_connection)
		{
			_connection->disconnect();
		}
	}
	
	void set(const connection<Args...>& other)
	{
		_connection = std::make_shared<connection<Args...> >(other);
	}
protected:
	std::shared_ptr<connection<Args...>> _connection;
};

template <typename ... Args>
class method
{
public:
#ifdef _MSC_VER
	method(const std::function<void(const Args&...)>& method)
		: _method(method)
	{
		
	}

	template <typename T>
	method(T* obj, void (T::*ptr_func)(const Args&...))
		: method(obj, ptr_func, make_int_sequence<sizeof...(Args)>{})
	{
		
	}

	template <typename T, int ... Is>
	method(T* obj, void (T::*ptr_func)(const Args&...), int_sequence<Is...>)
		: method(std::bind(ptr_func, obj, placeholder_template<Is>{}...))
	{
		
	}
#else
	method(const std::function<void(Args&&...)>& method)
		: _method(method)
	{
		
	}

	template <typename T>
	method(T* obj, void (T::*ptr_func)(Args&&...))
		: method(obj, ptr_func, make_int_sequence<sizeof...(Args)>{})
	{
		
	}

	template <typename T, int ... Is>
	method(T* obj, void (T::*ptr_func)(Args&&...), int_sequence<Is...>)
		: method(std::bind(ptr_func, obj, placeholder_template<Is>{}...))
	{
		
	}
#endif

#ifdef _MSC_VER
	void operator()(Args ... data)
	{
		_method(data...);
	}
	
	template <typename = std::enable_if_t<std::is_lvalue_reference<Args...>::value>>
#endif
	void operator()(Args&&... data)
	{
		_method(std::forward<Args>(data)...);
	}
		
protected:
#ifdef _MSC_VER
	std::function<void(const Args&...)> _method;
#else
	std::function<void(Args...)> _method;
#endif
};

template <typename ... Args>
class callback
{
public:
	using vector_methods = typename connection<Args...>::vector_methods;
	
	callback()
	{
		
	}
	
#ifdef _MSC_VER
	template <typename T>
	inline connection<Args...> connect(T* obj, void (T::*ptr_func)(const Args&...))
	{
		return _connect(obj, ptr_func, make_int_sequence < sizeof...(Args) > {});
	}

	inline connection<Args...> connect(const std::function<void(const Args&...)>& method)
	{
		typename vector_methods::iterator it = _registered.emplace(_registered.end(), method);
		return connection<Args...>(_registered, it);
	}
#else
	template <typename T>
	inline connection<Args...> connect(T* obj, void (T::*ptr_func)(Args&&...))
	{
		return _connect(obj, ptr_func, make_int_sequence<sizeof...(Args)>{});
	}

	inline connection<Args...> connect(const std::function<void(Args&&...)>& method)
	{
		typename vector_methods::iterator it = _registered.emplace(_registered.end(), method);
		return connection<Args...>(_registered, it);
	}
#endif

#ifdef _MSC_VER
	void operator()(Args ... data)
	{
		for(auto& reg : _registered)
		{
			reg(data...);
		}
	}
	
	template <typename = std::enable_if_t<std::is_lvalue_reference<Args...>::value>>
#endif
	void operator()(Args&& ... data)
	{
		for(auto& reg : _registered)
		{
			reg(std::forward<Args>(data)...);
		}
	}

protected:	
#ifdef _MSC_VER
	template <typename T, int ... Is>
	inline connection<Args...> _connect(T* obj, void (T::*ptr_func)(const Args&...), int_sequence<Is...>)
	{
		typename vector_methods::iterator it = _registered.emplace(_registered.end(), std::bind(ptr_func, obj, placeholder_template<Is>{}...));
		return connection<Args...>(_registered, it);
	}
#else
	template <typename T, int ... Is>
	inline connection<Args...> _connect(T* obj, void (T::*ptr_func)(Args&&...), int_sequence<Is...>)
	{
		typename vector_methods::iterator it = _registered.emplace(_registered.end(), std::bind(ptr_func, obj, placeholder_template<Is>{}...));
		return connection<Args...>(_registered, it);
	}
#endif
	
protected:
	vector_methods _registered;
};

template <typename ... Args>
class message
{
public:
	message(int priority, std::chrono::system_clock::time_point timestamp, Args&&... data)
		: _data(std::forward<Args>(data)...)
		, _priority(priority)
		, _timestamp(timestamp)
	{
		
	}

	message(message<Args...>&& other)
		: _priority(other._priority)
		, _timestamp(std::move(other._timestamp))
		, _data(std::move(other._data))
	{
		//std::cout << "constructor move message" << std::endl;
	}

	message<Args...>& operator=(message<Args...>&& other)
	{
		//std::cout << "asignation move message" << std::endl;

		_priority = other._priority;
		_timestamp = std::move(other._timestamp);
		_data = std::move(other._data);

		return *this;
	}

	message(const message<Args...>& other)
		: _priority(other._priority)
		, _timestamp(other._timestamp)
		, _data(other._data)
	{
		//std::cout << "constructor copy message" << std::endl;
	}
	
	message<Args...>& operator=(const message<Args...>& other)
	{
		//std::cout << "asignation copy message" << std::endl;

		_priority = other._priority;
		_timestamp = other._timestamp;
		_data = other._data;

		return *this;
	}
	
	~message()
	{
		//std::cout << "destructor message" << std::endl;
	}

	bool operator<(const message<Args...>& other) const
	{
		if (_timestamp < other._timestamp)
			return false;
		else if (_timestamp > other._timestamp)
			return true;
		
		return (_priority < other._priority);
	}
	
	int _priority;
	std::chrono::system_clock::time_point _timestamp;
	std::tuple<Args...> _data;
};

template <typename ... Args>
class queue_delayer
{
public:
	// deque vs vector: http://www.gotw.ca/gotw/054.htm
	using container_type = std::priority_queue<message<Args...>, std::deque<message<Args...> > >;
	
	queue_delayer()
	{
		
	}
	
#ifdef _MSC_VER
	template <typename R, typename P>
	void operator()(int priority, std::chrono::duration<R,P> delay, Args ... data)
	{
		auto delay_point = std::chrono::high_resolution_clock::now() + delay;
		_queue.emplace(priority, delay_point, std::move(data)...);
	}
	
	template <typename R, typename P, typename = std::enable_if_t<std::is_lvalue_reference<Args...>::value>>
#else
	template <typename R, typename P>
#endif
	void operator()(int priority, std::chrono::duration<R,P> delay, Args&& ... data)
	{
		auto delay_point = std::chrono::high_resolution_clock::now() + delay;
		_queue.emplace(priority, delay_point, std::forward<Args>(data)...);
	}
	
	void update()
	{
		auto t1 = std::chrono::high_resolution_clock::now();
		while(!_queue.empty())
		{
			// future proposal top: 
			// https://groups.google.com/a/isocpp.org/forum/#!msg/std-proposals/TIst1FOdveo/D54Uo-QuGfUJ
			auto& top = const_cast<typename container_type::value_type&>(_queue.top());
			//auto& top = _queue.top();
			if(t1 >= top._timestamp)
			{
				update(std::move(top), gens<sizeof...(Args)>{} );
				_queue.pop();
			}
			else
			{
				// all is already dispatched
				break;
			}
		}
	}
	
	template <typename T>
#ifdef _MSC_VER
	inline connection<Args...> connect(T* obj, void (T::*ptr_func)(const Args&...))
	{
		return _output.connect(obj, ptr_func);
	}
#else
	inline connection<Args...> connect(T* obj, void (T::*ptr_func)(Args&&...))
	{
		return _output.connect(obj, ptr_func);
	}
#endif

#ifdef _MSC_VER
	inline connection<Args...> connect(const std::function<void(const Args&...)>& method)
	{
		return _output.connect(method);
	}
#else
	inline connection<Args...> connect(const std::function<void(Args&&...)>& method)
	{
		return _output.connect(method);
	}
#endif
protected:
	template<int ...S>
	inline void update(typename container_type::value_type&& top, seq<S...>)
	{
		_output(std::forward<Args>(std::get<S>(top._data))...);
	}

protected:
	callback<Args... > _output;
	container_type _queue;
};

template <typename ... Args>
class queue_fast
{
public:
	using container_type = std::queue<std::tuple<Args...>, std::deque<std::tuple<Args...> > >;
	
	queue_fast()
	{
		
	}
	
#ifdef _MSC_VER
	void operator()(Args ... data)
	{
		_queue.emplace(std::move(data)...);
	}
	
	template <typename = std::enable_if_t<std::is_lvalue_reference<Args...>::value>>
#endif
	void operator()(Args&& ... data)
	{
		_queue.emplace(std::forward<Args>(data)...);
	}
	
	void update()
	{
		while(!_queue.empty())
		{
			update(std::move(_queue.front()), gens<sizeof...(Args)>{} );
			_queue.pop();
		}
	}

	template <typename T>
#ifdef _MSC_VER
	inline connection<Args...> connect(T* obj, void (T::*ptr_func)(const Args&...))
	{
		return _output.connect(obj, ptr_func);
	}
#else
	inline connection<Args...> connect(T* obj, void (T::*ptr_func)(Args&&...))
	{
		return _output.connect(obj, ptr_func);
	}
#endif

#ifdef _MSC_VER
	inline connection<Args...> connect(const std::function<void(const Args&...)>& method)
	{
		return _output.connect(method);
	}
#else
	inline connection<Args...> connect(const std::function<void(Args&&...)>& method)
	{
		return _output.connect(method);
	}
#endif

protected:	
	template<int ...S>
	inline void update(typename container_type::value_type&& top, seq<S...>)
	{
		_output(std::forward<Args>(std::get<S>(top))...);
	}

protected:
	callback<Args...> _output;
	container_type _queue;
};

} // end namespace

#endif
