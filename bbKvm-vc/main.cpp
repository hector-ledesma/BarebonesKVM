#include "Client.h"
#include "Server.h"
#include <iostream>
#include <vector>
#include <string>


std::vector<std::string>
splitCmds(std::string& cmd)
{
	std::vector<std::string> cmds;
	std::string delimiter = " ";
	auto limit = cmd.find(delimiter);

	if (limit != std::string::npos)
	{
		int start = 0;
		int end = limit;

		while (end != -1)
		{
			std::string sub = cmd.substr(start, end - start);
			if (!sub.empty())
				cmds.push_back(sub);

			start = end + delimiter.size();
			end = cmd.find(delimiter, start);
		}
	}

	return cmds;
}


int ServerApp()
{
	try
	{
		Server server;
		server.run();
	}
	catch (int err)
	{
		std::cerr << "Exiting program with error: " << err << std::endl;
		std::cout << "Enter something to exit." << std::endl;
		std::getchar();
		return err;
	}
	std::cout << "Enter something to exit." << std::endl;
	std::getchar();
	return 0;
}

int ClientApp(const char* ip)
{
	try
	{
		Client client(ip);
		client.run();
	}
	catch (int err)
	{
		std::cerr << "Exiting program with error: " << err << std::endl;
		std::cout << "Enter something to exit." << std::endl;
		std::getchar();
		return err;
	}
	std::cout << "Enter something to exit." << std::endl;
	std::getchar();
	return 0;
}

int main(int argc, char** argv)
{
	const char* welcomeStr = 
		"-------------------------------------------------------------------------------------------"
		"\n| Hello! Welcome to Barebones KVM, a proof of concept personal project by Hector Ledesma. |"
		"\n| Project page: https://github.com/hector-ledesma/BarebonesKVM                            |"
		"\n-------------------------------------------------------------------------------------------"
		
		"\n Options:"
		"\n\t ---- To run Server: \t 'server'"
		"\n\t ---- To run Client: \t 'client $SERVER_IP' (e.g.: client 192.168.x.x)"
		"\n\t ---- To exit: \t\t no input"
		;
	std::cout << welcomeStr << std::endl;

	bool runServer = false, runClient = false;
	std::string ip;
	std::string raw;

	while (!runServer && !runClient)
	{
		std::cout << "> ";
		std::getline(std::cin, raw);
		raw = raw + " ";
		std::vector<std::string> input = splitCmds(raw);

		if (input.size() > 0)
		{
			if (input.at(0) == "server")
				runServer = true;
			else if (input.at(0) == "client")
			{
				if (input.size() == 1)
					std::cout << "Please enter ip address in the same line." << std::endl;
				else
				{
					ip = input.at(1);
					runClient = true;
				}
			}
			else
				std::cout << "Sorry, can't recognize " << "' " << input.at(0) << " '" << " as an input. Try again." << std::endl;
		}
		else
			return 0;
	}

	if (runServer)
		return ServerApp();
	if (runClient)
		return ClientApp(ip.c_str());

}
