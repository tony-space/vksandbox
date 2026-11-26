#include "vkApplication.hpp"

#include <stdexcept>
#include <iostream>

int main()
{
	try
	{
		vkApplication app;
		app.run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << '\n';
	}

	return 0;
}
