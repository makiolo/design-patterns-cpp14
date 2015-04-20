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

namespace lead {

template <typename T> using CommandTalker = std::function<void(T&)>;
template <typename T> using CommandTalkerReturn = decltype(CommandTalker<T>);
template <typename T> using CompositeCommandTalker = std::function<CommandTalker<T>(const CommandTalker<T>&)>;

template <typename T>
CommandTalker<T> operator>>=(const CompositeCommandTalker<T>& a, const CommandTalker<T>& b)
{
	return a(b);
}

template <typename T>
CompositeCommandTalker<T> repeat(int n)
{
	return [=](const CommandTalker<T>& f)
	{
		return [&](T& self)
		{
			// I want coroutines :(
			for(int cont = 0; cont < n; ++cont)
			{
				f(self);
			}
		};
	};
}

template <typename T>
class talker
{
public:
	using container = fes::queue_delayer<CommandTalker<T> >;
	
	talker(const std::string& name)
		: _name(name)
		, _thread(nullptr)
	{
		
	}
	~talker()
	{
		
	}
	
	void add_follower(T& talker)
	{
		_conns.emplace_back(_queue.connect(std::bind(&talker::planificator, *this, std::ref(talker), std::placeholders::_1)));
	}
	
	void planificator(T& talker, const CommandTalker<T>& cmd)
	{
		if (_thread == nullptr)
		{
			//printf("creating new task\n");
			std::packaged_task<int(T&)> _task([&](T& parm){cmd(parm); return 0; });
			_future = _task.get_future();
			_thread = std::make_shared<std::thread>(std::move(_task), talker);
			_thread->detach();
		}
		else
		{
			// release cpu
			bool is_ready = _future.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready;
			if (is_ready)
			{
				//printf("task finished\n");
				auto ret = _future.get();
				_thread = nullptr;

				// planification now!
				planificator(talker, cmd);
			}
			else
			{
				// queue message with more priority
				order(cmd, 0, 1);
			}
		}
	}
	
	inline void order(const CommandTalker<T>& command, int milli = 0, int priority = 0)
	{
		_queue(priority, std::chrono::milliseconds(milli), command);
	}
	
	inline void update()
	{
		_queue.update();
	}
	
protected:
	std::vector<fes::connection_shared<CommandTalker<T> > > _conns;
	container _queue;
	std::string _name;
	std::shared_ptr<std::thread> _thread;
	std::shared_future<int> _future;
};

}


class Context
{
public:
	Context()
	{
		
	}
	
	~Context() { ; }
	
	// skills
	void print(const std::string& text)
	{
		std::cout << "thread: " << std::thread::id() << ": " << text << std::endl;
	}
protected:
};

class Buyer;

// client side
// ShopKeeper can be a lider of liders
class ShopKeeper : public lead::talker<Buyer>
{
public:
	ShopKeeper(const std::string& name)
		: talker(name)
	{
		
	}
	
	~ShopKeeper() { ; }
	
	// skills
	void say(const std::string& text)
	{
		std::cout << "thread: " << std::thread::id() << " <" << _name << "> " << text << std::endl;
	}
protected:
	//lead::talker<Context> _context;
};


class Buyer : public lead::talker< ShopKeeper >
{
public:
	Buyer(const std::string& name)
		: talker(name)
	{
		
	}
	~Buyer() { ; }	

	// skills
	void say(const std::string& text)
	{
		std::cout << "thread: " << std::thread::id() << " <" << _name << "> " << text << std::endl;
	}
protected:
	//lead::talker<Context> _context;
};

int main()
{
	{
		//lead::talker<Context> _context;
		auto buyer = Buyer("buyer");
		auto shopkeeper = ShopKeeper("shopkeeper");
		
		buyer.add_follower(shopkeeper);
		shopkeeper.add_follower(buyer);
		
		// conversation between tyrants
		
		buyer.order([=](ShopKeeper& self) {
				self.say("Hi!");
		}, 100);

		shopkeeper.order([=](Buyer& self) {
				self.say("How much cost are the apples?");
		}, 1100);

		buyer.order([=](ShopKeeper& self) {
				self.say("50 cents each apple"); 
		}, 2500);

		shopkeeper.order([=](Buyer& self) {
				self.say("I want apples!");
		}, 3100);

		buyer.order([=](ShopKeeper& self) {
				self.say("You apples, thanks"); 
		}, 4500);

		shopkeeper.order([=](Buyer& self) {
				self.say("Bye!");
		}, 6000);
		
		for(int i=0; i<900 /* x 10 */; ++i)
		{
			// process queue
			buyer.update();
			shopkeeper.update();

			std::this_thread::sleep_for( std::chrono::milliseconds(10) );
		}
	}
	return(0);
}

