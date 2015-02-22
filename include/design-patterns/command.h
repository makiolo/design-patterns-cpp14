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
	template <typename F, typename... Args>
	Command(F&& func, Args&&... args)
		: _function(std::forward<F>(func)),
		_args(std::forward<Args>(args)...)
	{
		;
	}

	template <typename... Args, int... Is>
	ret execute(std::tuple<Args...>& tuple, seq<Is...>)
	{
		return _function(std::get<Is>(tuple)...);
	}

	template <typename... Args>
	ret execute(std::tuple<Args...>& tuple)
	{
		return execute(tuple, gens < sizeof...(Args) > {});
	}

	ret operator()()
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
