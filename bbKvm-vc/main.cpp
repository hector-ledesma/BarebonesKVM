#include "Server.h"
#include <iostream>

int main()
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