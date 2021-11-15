#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"


void 
main() {
	WSADATA wsaData;
	int wsOk = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsOk != 0)
	{
		std::cerr << "WSAStartup failed: " << wsOk << std::endl;
	}
	else
	{
		std::cout << "WSAStartup successful" << std::endl;
	}

	//	Set up our addrinfo structure to request server information.
	addrinfo hints, 
		* result;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			// MS Docs use AF_UNSPEC, I'm certain I want ot use IPv4 so I'll go with AF_INET.
	hints.ai_socktype = SOCK_STREAM;	// Streaming socket
	hints.ai_protocol = IPPROTO_TCP;	// TCP/IP protocol

	//	Resolve the server address and port.
	int iResult;
	/*
		I'll be developing the program using the same host, or another local machine I have full control of, so for the first param of geaddrinfo() I can specify 'localhost' or my other machine's ip.

		TODO:
		Figure out how to  find a server within your local network without knowing its domain name lol.
	*/
	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cerr << "getaddrinfo failed: " << iResult << std::endl;
		WSACleanup();
		return;
	}
	else
	{
		std::cout << "LocalHost resolves successfully successful:\n"
			<< "\t|--- Address: \t" << result->ai_addr << "\n"
			<< "\t|--- Family: \t" << result->ai_family << "\n"
			<< "\t|--- SckType: \t" << result->ai_socktype << "\n"
			<< "\t|--- Protocol: \t" << result->ai_protocol << "\n";
	}

	freeaddrinfo(result);
	WSACleanup();
}