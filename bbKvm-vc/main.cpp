#include "Server.h"
#include <iostream>

int main_main()
{
	try
	{
		Server server;
		server.run();
	}
	catch (int err)
	{
		std::cerr << err << std::endl;
		return err;
	}

	return 0;
}