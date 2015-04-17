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
#ifdef _WIN32
#include <Windows.h>
#endif

namespace lead {

template <typename T> using CommandTalker = std::function<void(T&)>;
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
	{
		
	}
	~talker() { ; }
	
	void add_follower(T& talker)
	{
		_conns.emplace_back( get_queue().connect(std::bind(&talker::planificator, *this, std::ref(talker), std::placeholders::_1)) );
	}
	
	void planificator(T& talker, const CommandTalker<T>& cmd)
	{
		//TODO:
		//TODO:
		//TODO:
		//
		// command added to queue
		// El que encola establece sus propias prioridades
		// Al despachar se vuelve a encolar con las
		// preferencias personales
		//
		// Si no se esta ejecutando nada:
		//		se ejecuta
		// Si ya se esta ejecutando algo, algoritmos de planificación:
		//		Cuando una nueva tarea cancela la que se esta ejecutando?
		try
		{
			auto resul = std::async(std::launch::async, [&]{ cmd(talker); });
			resul.get();
		}
		catch(...)
		{
			
		}
		/*
		if I am idle:
			trabajar
		else
			if nueva tarea es mas importante que la actual
				cancelar actual y trabajar con la nueva
			else
				encolar la nueva tarea con una prioridad basada en heuristica
		
		// Otra opcion es asumir que los comandos son latentes
		*/
	}
	
	void order(const CommandTalker<T>& command, int milli = 0, int priority = 0)
	{
		_queue(priority, std::chrono::milliseconds(milli), command);
	}
	
	container& get_queue()
	{
		return _queue;
	}
	
	void update()
	{
		_queue.update();
	}

	void suspend()
	{
		std::cout << "suspend" << std::endl;
	}
	
	void resume()
	{
		std::cout << "resume" << std::endl;
	}
	
protected:
	std::vector<fes::connection_shared<CommandTalker<T> > > _conns;
	container _queue;
	std::string _name;
};

}

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
		std::cout << "<" << _name << "> " << text << std::endl;
		//return true;
	}
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
		std::cout << "<" << _name << "> " << text << std::endl;
		//return true;
	}
};

int main()
{
	{
		auto buyer = Buyer("buyer");
		auto shopkeeper = ShopKeeper("shopkeeper");
		
		buyer.add_follower(shopkeeper);
		shopkeeper.add_follower(buyer);
		
		// conversation between tyrants
		
		int time = 0;
		buyer.order([=](ShopKeeper& self) {
				self.say("Hi!");
		}, time++);

		shopkeeper.order([=](Buyer& self) {
				self.say("How much cost are the apples?");
		}, time++);

		buyer.order([=](ShopKeeper& self) {
				self.say("50 cents each apple"); 
		}, time++);

		shopkeeper.order([=](Buyer& self) {
				self.say("I want apples!");
		}, time++);
		
		for(int i=0; i<1000; ++i)
		{
			// process queue
			buyer.update();
			shopkeeper.update();
#ifdef _WIN32
			Sleep(1);
#endif
		}
	}
	return(0);
}

