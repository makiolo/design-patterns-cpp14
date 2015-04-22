// Ricardo Marmolejo Garcia
// 15-04-2015
// testing fes
#include <iostream>
#include <tuple>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <assert.h>
#include <fast-event-system/fes.h>

template <typename TYPE = fes::callback<std::string> >
class Producer
{
public:
	Producer() {}
	~Producer() {}

	void operator()(const std::string& data)
	{
		_channel(data);
	}

	template <typename R, typename P>
	void operator()(int priority, std::chrono::duration<R,P> delay, const std::string& data)
	{
		_channel(priority, delay, data);
	}

	void update()
	{
		_channel.dispatch();
	}
	
	TYPE& get_channel() {return _channel;}
protected:
	TYPE _channel;
};

template <typename TYPE = fes::callback<std::string> >
class Consumer
{
public:
	Consumer()
		: _data("waiting")
	{
		
	}

	~Consumer()
	{
		
	}

	void connect(Producer<TYPE>& producer)
	{
		_conn = producer.get_channel().connect(this, &Consumer::on_handler);
	}
	
	const std::string& get_data() const {return _data;}

protected:
	void on_handler(const std::string& data)
	{
		_data = data;
	}
	
	std::string _data;
	fes::shared_connection<std::string> _conn;
};

int main()
{
	{
		Producer<fes::callback<std::string> > p;
		Consumer<fes::callback<std::string> > c1;
		Consumer<fes::callback<std::string> > c2;
		{
			c1.connect(p);
			c2.connect(p);
			p("data");
			assert(c1.get_data() == "data");
			assert(c2.get_data() == "data");
		}
	}
	{
		Producer<fes::queue_fast<std::string> > p;
		Consumer<fes::queue_fast<std::string> > c1;
		Consumer<fes::queue_fast<std::string> > c2;
		{
			c1.connect(p);
			c2.connect(p);
			p("data");
			p.update();
			assert(c1.get_data() == "data");
			assert(c2.get_data() == "data");
		}
	}
	{
		Producer<fes::queue_delayer<std::string> > p;
		Consumer<fes::queue_delayer<std::string> > c1;
		Consumer<fes::queue_delayer<std::string> > c2;
		{
			c1.connect(p);
			c2.connect(p);
			p(0, std::chrono::milliseconds(0), "data");
			p.update();
			assert(c1.get_data() == "data");
			assert(c2.get_data() == "data");
		}
	}
	
	return 0;
}

