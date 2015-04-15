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
using CommandUBot = std::function<void(UBot&)>;
using CompositeCommandUBot = std::function<CommandUBot(const CommandUBot&)>;

CommandUBot operator+=(const CompositeCommandUBot& a, const CommandUBot& b)
{
	return a(b);
}

CompositeCommandUBot repeat(int n)
{
	return [=](const CommandUBot& f)
	{
		return [&](UBot& self)
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
	
	void add_follower(UBot& follower)
	{
		//_conns.push_back(
		get_queue().connect([&](CommandUBot&& cmd)
		{
			cmd(follower);
		})
		//)
		;
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
	//std::vector<fes::connection_scoped<CommandUBot>> _conns;
	fes::callback<CommandUBot> _queue;
	std::string _name;
};

int main()
{
	auto leader1 = std::make_shared<UBot>("leader 1");
	auto leader2 = std::make_shared<UBot>("leader 2");
	auto follower1 = std::make_shared<UBot>("follower 1");
	auto follower2 = std::make_shared<UBot>("follower 2");
	
	leader1->add_follower(*follower1);
	leader1->add_follower(*follower2);
	
	leader1->order([=](UBot& self) {
									self.right(); 
								});
	leader1->order(repeat(2) += [=](UBot& self) {
									self.left();
								});
	leader1->order(repeat(2) += [=](UBot& self) {
									self.up(); 
								});
	leader1->order(repeat(2) += [=](UBot& self) {
									self.down(); 
								});
	leader1->order(repeat(2) += [=](UBot& self) {
									self.right(); 
								});
	
	// follower threat your leader
	follower2->add_follower(*leader2);
	follower2->order(repeat(5) += [=](UBot& self) {
									self.kamikace();
									self.kamikace();
								});
	return(0);
}

