#include "TCPSocketController.h"
#include "SocketData.h"

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

/*
	No WinSock means no networking, which means no program.
	So, initialize WinSock on constructor, and just exit if it fails.
*/
TCPSocketController::TCPSocketController(const char* port, const char* address) 
	: m_port(port), m_address(address) {
	std::cout << "\n[TCPSocketController] ---- Constructor called." << std::endl;
	try {
		WSADATA wsaData;
		int wsOk = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (wsOk != 0)
			throw wsOk;
	}
	catch (int e)
	{
		std::cerr << "\n[TCPSocketController] ---- WSAStartup failed: " << e << std::endl;
	}

	std::cout << "\n[TCPSocketController] ---- WSAStartup successful" << std::endl;
}

/*
	Clean up WinSock.
	We'll handle cleaning addrinfo objects, as well as the sockets, in a separate container class.
*/
TCPSocketController::~TCPSocketController() {
	std::cout << "\n[TCPSocketController] ---- Destructor called. Cleaning up WSA." << std::endl;

	WSACleanup();
}

const char*
TCPSocketController::getPort()
{
	return m_port;
}

SocketData
TCPSocketController::generateSocket(bool isServer) {
	std::cout << "\n[TCPSocketController] ---- Generating socket." << std::endl;

	SocketData socketContainer;
	addrinfo hints, * result;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			//	IPv4
	hints.ai_socktype = SOCK_STREAM;	//	Stream Socket
	hints.ai_protocol = IPPROTO_TCP;	//	TCP Protocol
	if (isServer)
		hints.ai_flags = AI_PASSIVE;	//	Caller will use returned socket in bind() call. Server listening socket.


	try
	{
		socketContainer.initSocket(getPort(), hints);
	}
	catch (int e)
	{
		throw e;
	}
	
	std::cout << "\n[TCPSocketController] ---- Generated Socket Data:" << std::endl;
	std::cout << "\t |---- Socket address:" << socketContainer.getAddr() << std::endl;
	std::cout << "\t |---- Socket socket:" << socketContainer.getSocket() << std::endl;

	return socketContainer;
}
