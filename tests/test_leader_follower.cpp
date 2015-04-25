// Ricardo Marmolejo Garcia
// 15-04-2015
// experimental reinterpretation pattern command
#include <iostream>
#include <tuple>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <fast-event-system/fes.h>
#include <future>
#include <chrono>
#include <assert.h>
#include <atomic>
#include <exception>
#include <mutex>

namespace lead {

template <typename T> using CommandTalker = std::function<void(T&)>;
template <typename T> using CompositeCommandTalker = std::function<CommandTalker<T>(const CommandTalker<T>&)>;

template <typename SELF, typename T>
class scheduler
{
public:
	using command_other = CommandTalker<T> ;
	using command_reflexion = CommandTalker<SELF> ;
	using container_other = fes::queue_delayer<command_other>;
	using container_reflexion = fes::queue_delayer<command_reflexion>;
	
	scheduler(const std::string& name)
		: _name(name)
		, _busy_other(false)
		, _busy_me(false)
	{
		_conn_me = _commands_me.connect(std::bind(&scheduler::planificator_me, this, std::placeholders::_1));
	}
	~scheduler()
	{
		
	}

	scheduler(const scheduler&) = delete;
	scheduler& operator=(const scheduler&) = delete;
	
	void add_follower(T& talker)
	{
		_conns.emplace_back(_commands_other.connect(std::bind(&scheduler::planificator_other, this, std::ref(talker), std::placeholders::_1)));
	}
	
	void planificator_me(const command_reflexion& cmd)
	{
		std::thread th([&]()
		{
			cmd(static_cast<SELF&>(*this));
			_busy_me = false;
		});
		th.detach();
	}

	void planificator_other(T& talker, const command_other& cmd)
	{
		std::thread th([&]()
		{
			cmd(talker);
			_busy_other = false;
		});
		th.detach();
	}

	inline void call_async(const command_reflexion& command, int milli = 0, int priority = 0)
	{
		_commands_me(priority, std::chrono::milliseconds(milli), command);
	}

	inline void call_followers(const command_other& command, int milli = 0, int priority = 0)
	{
		_commands_other(priority, std::chrono::milliseconds(milli), command);
	}
	
	void update()
	{
		if (!_busy_other)
		{
			_busy_other = _commands_other.dispatch();
		}
		if (!_busy_me)
		{
			_busy_me = _commands_me.dispatch();
		}
	}

	inline void sleep(int milli)
	{
		std::this_thread::sleep_for( std::chrono::milliseconds(milli) );
	}

	void signal()
	{
		_signal.notify_one();
	}
	
	void wait()
	{
		std::unique_lock<std::mutex> context(_signal_lock);
		_signal.wait(context);
	}
protected:
	std::string _name;
	// me self
	fes::shared_connection<command_reflexion> _conn_me;
	container_reflexion _commands_me;
	std::atomic<bool> _busy_me;
	// others
	std::vector<fes::shared_connection<command_other> > _conns;
	container_other _commands_other;
	std::atomic<bool> _busy_other;
	// 
	std::condition_variable _signal;
	std::mutex _signal_lock;
};

}

class Context
{
public:
	explicit Context()
	{
		
	}
	
	~Context() { ; }

	inline void sleep(int milli)
	{
		std::this_thread::sleep_for( std::chrono::milliseconds(milli) );
	}

	void print(const std::string& name, const std::string& text, int delay = 10)
	{
		std::unique_lock<std::mutex> context(_lock);

		std::cout << "<" << name << "> ";
		for(const char& c : text)
		{
			std::cout << c << std::flush;
			sleep(delay);
		}
		std::cout << std::endl;
	}
protected:
	std::mutex _lock;
};

class Buyer;

class ShopKeeper : public lead::scheduler<ShopKeeper, Buyer>
{
public:
	explicit ShopKeeper(const std::string& name, Context& context)
		: scheduler(name)
		, _context(context)
	{
		
	}
	
	~ShopKeeper() { ; }
	
	void say(const std::string& text, int delay = 10)
	{
		_context.print(_name, text, delay);
	}
protected:
	Context& _context;
};


class Buyer : public lead::scheduler<Buyer, ShopKeeper>
{
public:
	explicit Buyer(const std::string& name, Context& context)
		: scheduler(name)
		, _context(context)
	{
		
	}
	~Buyer() { ; }	

	void say(const std::string& text, int delay = 10)
	{
		_context.print(_name, text, delay);
	}
protected:
	Context& _context;
};

/*
lead::CommandTalker<Buyer> operator>>=(const lead::CompositeCommandTalker<Buyer>& a, const lead::CommandTalker<Buyer>& b)
{
	return a(b);
}

lead::CompositeCommandTalker<Buyer> repeat(int n)
{
	return [=](const lead::CommandTalker<Buyer>& f)
	{
		return [&](Buyer& self)
		{
			for(int cont = 0; cont < n; ++cont)
			{
				f(self);
			}
		};
	};
}
*/

int main()
{
	std::ios_base::sync_with_stdio(false);

	{
		Context context;
		Buyer buyer("tu", context);
		ShopKeeper shopkeeper("shopkeeper", context);
		shopkeeper.add_follower(buyer);
		
		buyer.call_async([&](Buyer& self) {
			self.say("Te despiertas en la carcel. No te acuerdas de nada.");
			self.sleep(100);
			self.say("	1. Examinar la celda");
			self.sleep(100);
			self.say("	2. Seguir durmiendo");
			self.sleep(100);

			int choose = -1;
			while (choose == -1)
			{
				if (!(std::cin >> choose))
				{
					std::cout << "Option not is a number" << std::endl;
				}
				else if (choose == 1)
				{
					self.call_async([&](Buyer& self) {
						self.say("Has elegido: Examinar la habitacion");
						self.sleep(100);
						self.say("Solo hay una manta sucia y un orinal");
						self.sleep(100);
					});
				}
				else if (choose == 2)
				{
					self.call_async([&](Buyer& self) {
						self.say("Has elegido: seguir durmiendo!");
						self.sleep(100);
						self.say("ZzZzZz ...", 500);
						self.sleep(200);
					});
				}
				else
				{
					std::cout << "Invalid option" << std::endl;
				}
			}

		});
		
		
#ifdef _WIN32
		while (true)
#else
		for (int i = 0; i < 9000; ++i)
#endif
		{
			buyer.update();
			shopkeeper.update();
			//
			shopkeeper.sleep(1);
		}
	}
	return(0);
}

