#ifndef _COMMANDER_H_
#define _COMMANDER_H_

#include "common.h"

namespace dp14 {

template <typename ret = void, typename... Ts>
class Command
{
private:
	std::function<ret(Ts...)> _function;
	std::tuple<Ts...> _args;
public:

#ifdef _MSC_VER
	template <typename F>
	Command(F&& func, Ts&&... args)
		: _function(std::forward<F>(func)) 
		, _args(std::forward<Ts>(args)...)
	{
		
	}
	template <typename = std::enable_if_t<std::is_lvalue_reference<Args...>::value>, typename F, typename... Args>
 #else
	template <typename F, typename... Args>
#endif
	Command(F&& func, Args&&... args)
		: _function(std::forward<F>(func)) 
		, _args(std::forward<Args>(args)...)
	{
		
	}

	template <typename... Args, int... Is>
	ret execute(const std::tuple<Args...>& tuple, seq<Is...>) const
	{
		return _function(std::get<Is>(tuple)...);
	}

	template <typename... Args>
	ret execute(const std::tuple<Args...>& tuple) const
	{
		return execute(tuple, gens < sizeof...(Args) > {});
	}

	ret operator()() const
	{
		return execute(_args);
	}
};

template <typename ret = void, typename F, typename... Args>
Command<ret, Args...> make_command(F&& f, Args&&... args)
{
	return Command<ret, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
}

}

#endif
