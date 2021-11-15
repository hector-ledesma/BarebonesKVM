#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
//#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"

void main() {
	//	Initialize WinSock
	WSADATA wsaData;
	int wsOk = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsOk != 0)
	{
		std::cerr << "WSAStartup failed: " << wsOk << std::endl;
		return;
	} 
	else
	{
		std::cout << "WSAStartup successful" << std::endl;
	}

	addrinfo hints, *result;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	/*
		getaddrinfo() seems to do some crazy complex DNS lookup related stuff:
		https://jameshfisher.com/2018/02/03/what-does-getaddrinfo-do/

		But in our case, setting the pNodeName param to NULL serves to resolve our local address and port to be used by the server.

		We can then use this information to populate the info necessary for our socket and bind system calls.

		If we already possess all the info we need, we can skip straight to populating a sockaddr_in structure ourselves, as done in Sloan Kelly's tutorials:
		https://www.youtube.com/watch?v=WDn-htpBlnU
		https://stackoverflow.com/questions/23401147/what-is-the-difference-between-struct-addrinfo-and-struct-sockaddr/23401272

		I'll stick to using MSDN's approach for now.
	*/
	wsOk = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (wsOk != 0)
	{
		std::cerr << "getaddrinfo failed: " << wsOk << std::endl;
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


	//	Clean up WinSock
	WSACleanup();
}