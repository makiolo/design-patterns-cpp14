#include <iostream>
#include <tuple>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <fast-event-system/fes.h>

class UBot
{
public:
	using CommandUBot = std::function < void(UBot*) >;
	
	UBot(const std::string& name)
		: _name(name)
	{
		
	}
	
	void update()
	{
		_queue.update();
	}
	
	void walk()
	{
		std::cout << "I am " << _name << ", state: walking" << std::endl;
	}
	
	void kamikace()
	{
		std::cout << "I am " << _name << ", state: kamikace" << std::endl;
	}
	
	fes::queue_fast<CommandUBot>& get_queue()
	{
		return _queue;
	}
	
	void connect(const std::shared_ptr<UBot>& leader)
	{
		// follow to leader
		_conn = leader->get_queue().connect([&](CommandUBot&& cmd)
		{
			cmd(this);
		});
	}
	
	void order()
	{
		_queue(
			[=](UBot* self)
			{
				self->walk();
				self->kamikace();
			}
		);
	}
protected:
	fes::connection_scoped<CommandUBot> _conn;
	fes::queue_fast<CommandUBot> _queue;
	std::string _name;
};

int main()
{
	auto leader = std::make_shared<UBot>("leader");
	auto follower1 = std::make_shared<UBot>("f1");
	auto follower2 = std::make_shared<UBot>("f2");
	follower1->connect(leader);
	follower2->connect(leader);
	leader->order();
	leader->update();
	// order himself
	follower1->connect(follower1);
	follower1->order();
	follower1->update();
	return(0);
}

