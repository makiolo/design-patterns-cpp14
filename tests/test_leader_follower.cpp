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
/*
#ifdef _WIN32
#include <Windows.h>
#endif
*/

namespace lead {

template <typename T> using CommandShopKeeper = std::function<bool(T&)>;
#if 0
template <typename T> using CompositeCommandShopKeeper = std::function<CommandShopKeeper<T>(const CommandShopKeeper<T>&)>;

template <typename T>
CommandShopKeeper<T> operator>>=(const CompositeCommandShopKeeper<T>& a, const CommandShopKeeper<T>& b)
{
	return a(b);
}

template <typename T>
CompositeCommandShopKeeper<T> repeat(int n)
{
	return [=](const CommandShopKeeper<T>& f)
	{
		return [&](T& self)
		{
			// I want coroutines :(
			int cont = 0;
			while(cont < n)
			{
				bool ret = f(self);
				if(ret)
				{
					++cont;
				}
			}
			return true;
		};
	};
}
#endif

template <typename T>
class talker
{
public:
	talker(const std::string& name)
		: _name(name)
	{
		
	}
	~talker() { ; }
	
	void add_shopkeeper(T& shopkeeper)
	{
		// TODO: save connection
		get_queue().connect(std::bind(&talker::planificator, *this, shopkeeper, std::placeholders::_1));
	}

	void planificator(T& shopkeeper, CommandShopKeeper<T>&& cmd)
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
		cmd(shopkeeper);
		
		/*
		if I am idle:
			trabajar
		else
			if nueva tarea es mas importante que la actual
				cancelar actual y trabajar con la nueva
			else
				encolar la nueva tarea con una prioridad basada en heuristica
		*/
	}
	
	/*	
	template <typename R, typename P>
	void order(int priority, std::chrono::duration<R,P> delay, const CommandShopKeeper& command)
	{
		_queue(priority, delay, command);
	}
	*/
	
	void order(const CommandShopKeeper<T>& command)
	{
		_queue(command);
	}
	
	fes::callback<CommandShopKeeper<T> >& get_queue()
	{
		return _queue;
	}
	
	/*
	void update()
	{
		_queue.update();
	}
	*/
	
	bool kamikace()
	{
		std::cout << "I am " << _name << ", state: kamikace" << std::endl;
		return true;
	}
	
protected:
	//std::vector<fes::connection_scoped<CommandShopKeeper>> _conns;
	fes::callback<CommandShopKeeper<T> > _queue;
	std::string _name;
};

}

// client side
// ShopKeeper can be a lider of liders
class ShopKeeper : public lead::talker< lead::talker<ShopKeeper> >
{
public:
	ShopKeeper(const std::string& name)
		: talker(name)
	{
		
	}
	
	~ShopKeeper() { ; }
	
	// skills
	bool right()
	{
		std::cout << "I am " << _name << ", state: right" << std::endl;
		return true;
	}

	bool left()
	{
		std::cout << "I am " << _name << ", state: left" << std::endl;
		return true;
	}

	bool up()
	{
		std::cout << "I am " << _name << ", state: up" << std::endl;
		return true;
	}

	bool down()
	{
		std::cout << "I am " << _name << ", state: down" << std::endl;
		return true;
	}
};


class Buyer : public lead::talker< lead::talker<ShopKeeper> >
{
public:
	Buyer(const std::string& name)
		: talker(name)
	{
		
	}
	~Buyer() { ; }	
};

int main()
{
	auto talker1 = std::make_shared<lead::talker<ShopKeeper> >("talker 1");
	auto talker2 = std::make_shared<lead::talker<ShopKeeper> >("talker 2");
	auto shopkeeper1 = std::make_shared<ShopKeeper>("shopkeeper 1");
	auto shopkeeper2 = std::make_shared<ShopKeeper>("shopkeeper 2");
	
	talker1->add_shopkeeper(*shopkeeper1);
	talker1->add_shopkeeper(*shopkeeper2);
	
	talker1->order([=](ShopKeeper& self) {return self.right();});
	talker1->order([=](ShopKeeper& self) {return self.left();});
	talker1->order([=](ShopKeeper& self) {return self.up();});
	talker1->order([=](ShopKeeper& self) {return self.down();});
	talker1->order([=](ShopKeeper& self) {return self.right();});
	
	// shopkeeper threat your talker
	shopkeeper2->add_shopkeeper(*talker2);
	shopkeeper2->order([=](lead::talker<ShopKeeper>& self) {return self.kamikace();});

	return(0);
}

