// Ricardo Marmolejo Garcia
// 15-04-2015
// reinterpretation pattern command
#include <iostream>
#include <tuple>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <fast-event-system/fes.h>
/*
#ifdef _WIN32
#include <Windows.h>
#endif
*/

class UBot;

using CommandUBot = std::function<void(UBot*)>;
using composite = std::function<CommandUBot(const CommandUBot&)>;

CommandUBot operator+=(const composite& a, const CommandUBot& b)
{
	return a(b);
}

composite repeat(int n)
{
	return [=](const CommandUBot& f)
	{
		return [&](UBot* self)
		{
			for(int i=0; i<n; ++i)
			{
				f(self);
			}
		};
	};
}

class UBot
{
public:
	UBot(const std::string& name)
		: _name(name)
	{
		
	}
		
	void connect(const std::shared_ptr<UBot>& leader)
	{
		_conn = leader->get_queue().connect([&](CommandUBot&& cmd)
		{
			cmd(this);
		});
	}

	/*	
	template <typename R, typename P>
	void order(int priority, std::chrono::duration<R,P> delay, const CommandUBot& command)
	{
		_queue(priority, delay, command);
	}
	*/
	void order(const CommandUBot& command)
	{
		_queue(command);
	}
	
	fes::callback<CommandUBot>& get_queue()
	{
		return _queue;
	}

	/*
	void update()
	{
		_queue.update();
	}
	*/
	
	// skills
	void right()
	{
		std::cout << "I am " << _name << ", state: right" << std::endl;
	}
	void left()
	{
		std::cout << "I am " << _name << ", state: left" << std::endl;
	}
	void up()
	{
		std::cout << "I am " << _name << ", state: up" << std::endl;
	}
	void down()
	{
		std::cout << "I am " << _name << ", state: down" << std::endl;
	}
	
	void kamikace()
	{
		std::cout << "I am " << _name << ", state: kamikace" << std::endl;
	}

protected:
	fes::connection_scoped<CommandUBot> _conn;
	fes::callback<CommandUBot> _queue;
	std::string _name;
};

int main()
{
	auto leader1 = std::make_shared<UBot>("leader1");
	auto leader2 = std::make_shared<UBot>("leader2");
	auto follower1 = std::make_shared<UBot>("follower1");
	auto follower2 = std::make_shared<UBot>("follower2");
	{
		follower1->connect(leader1);
		follower2->connect(leader1);
		leader1->order( [=](UBot* self) { self->right(); } );
		leader1->order(repeat(1) += [=](UBot* self) { self->left(); } );
		leader1->order(repeat(1) += [=](UBot* self) { self->up(); } );
		leader1->order(repeat(1) += [=](UBot* self) { self->down(); } );
		follower2->connect(leader2);
		leader2->order(repeat(100) += [=](UBot* self) { self->kamikace(); } );
		
		/*
		for(int i=0;i<4000;++i)
		{
			leader1->update();
			leader2->update();
#ifdef _WIN32
			Sleep(1);
#endif
		}
		*/
	}
	return(0);
}

