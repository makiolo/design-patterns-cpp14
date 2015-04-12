#include <iostream>
#include <tuple>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <queue>

#include <fast-event-system/fes.h>
#include <design-patterns/command.h>

class Follower;

using CommandFollower = std::function < void(Follower*) >;

class UBot
{
public:
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
		std::cout << "I am walking" << std::endl;
	}
	
	void kamikace()
	{
		std::cout << "I am kamikace" << std::endl;
	}
	
	fes::queue_fast<CommandFollower>& get_queue()
	{
		return _queue;
	}

protected:
	fes::queue_fast<CommandFollower> _queue;
	std::string _name;
};

class Follower : public UBot
{
public:
	Follower()
		: UBot("follower")
	{

	}

	void connect(const std::shared_ptr<UBot>& leader)
	{
		// follow to leader
		
		_conn = leader->get_queue().connect([&](const CommandFollower& cmd)
		{
			cmd(this);
		});
		
		//_conn = leader->get_queue().connect(this, &Follower::dispatch);
	}

	/*
	void dispatch(const CommandFollower& cmd)
	{
		cmd(this);
	}
	*/
	
protected:
	fes::connection_scoped<CommandFollower> _conn;
};

class Leader : public UBot
{
public:
	Leader()
		: UBot("leader")
	{
		
	}
	
	void order()
	{
		_queue(
			[=](Follower* self)
			{
				self->walk();
				self->kamikace();
			}
		);
	}
};

int main()
{
	auto leader = std::make_shared<Leader>();
	auto follower1 = std::make_shared<Follower>();
	auto follower2 = std::make_shared<Follower>();
	follower1->connect(leader);
	follower2->connect(leader);

	leader->order();

	for (int i = 0; i < 4000; ++i)
	{
		leader->update();
	}
	return(0);
}

