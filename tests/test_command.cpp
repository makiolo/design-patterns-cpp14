#include <iostream>
#include <sstream>
#include <assert.h>
#include <design-patterns/command.h>

int main()
{
	// command return void by default
	auto command_void = dp14::make_command([](int x, int y){std::cout << "x+y = " << x+y << std::endl;}, 10, 20);
	command_void();
	
	// you can specify a different return value
	auto command_int = dp14::make_command<int>([](int x, int y){return x+y;}, 10, 20);
	std::cout << "return: " << command_int() << std::endl;
	
	return(0);
}
