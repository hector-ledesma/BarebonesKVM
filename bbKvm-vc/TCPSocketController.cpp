#include "TCPSocketController.h"
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include "SocketData.h"

#pragma comment(lib, "Ws2_32.lib")


/*
	No WinSock means no networking, which means no program.
	So, initialize WinSock on constructor, and just exit if it fails.
*/
TCPSocketController::TCPSocketController(char* port, char* address) 
	: m_port(port), m_address(address) {
	try {
		WSADATA wsaData;
		int wsOk = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (wsOk != 0)
			throw wsOk;
	}
	catch (int e)
	{
		std::cerr << "WSAStartup failed: " << e << std::endl;
	}

	std::cout << "WSAStartup successful" << std::endl;
}

/*
	Clean up WinSock.
	We'll handle cleaning addrinfo objects, as well as the sockets, in a separate container class.
*/
TCPSocketController::~TCPSocketController() {
	WSACleanup();
}

SocketData*
TCPSocketController::generateSocket(bool isServer) {

	SocketData* socketContainer;
	addrinfo hints, * result;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			//	IPv4
	hints.ai_socktype = SOCK_STREAM;	//	Stream Socket
	hints.ai_protocol = IPPROTO_TCP;	//	TCP Protocol
	if (isServer)
		hints.ai_flags = AI_PASSIVE;	//	Caller will use returned socket in bind() call. Server listening socket.


	try
	{
		socketContainer = &SocketData(this, hints);
		socketContainer->initSocket();
	}
	catch (int e)
	{
		return nullptr;
	}
	
	return socketContainer;
}
