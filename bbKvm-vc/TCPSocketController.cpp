#include "TCPSocketController.h"

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

/*
	No WinSock means no networking, which means no program.
	So, initialize WinSock on constructor, and just exit if it fails.
*/
TCPSocketController::TCPSocketController(const char* port, const char* address)
	: m_port(port),
	m_address(address),
	m_result(nullptr)
{
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
	Clean up WinSock and our addrinfo pointer.

	We can also free up our addrinfo and set the pointer to nullptr once we have a socket successfully in a listening state.
*/
TCPSocketController::~TCPSocketController() {
	std::cout << "\n[TCPSocketController] ---- Destructor called." << std::endl;
	
	std::cout << "\t|----Freeing addrinfo." << std::endl;
	freeaddrinfo(m_result);

	std::cout << "\t|----Cleaning up WinSock." << std::endl;
	WSACleanup();
}

const char*
TCPSocketController::getPort()
{
	return m_port;
}

SOCKET
TCPSocketController::generateSocket(bool isServer) {
	std::cout << "\n[TCPSocketController] ---- Generating socket." << std::endl;

	SOCKET workSocket;
	addrinfo hints, * result;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			//	IPv4
	hints.ai_socktype = SOCK_STREAM;	//	Stream Socket
	hints.ai_protocol = IPPROTO_TCP;	//	TCP Protocol
	if (isServer)
		hints.ai_flags = AI_PASSIVE;	//	Caller will use returned socket in bind() call. Server listening socket.

	int wsOk = getaddrinfo(NULL, m_port, &hints, &m_result);
	if (wsOk != 0)
	{
		std::cerr << "\n[TCPSocketController] ---- geraddrinfo failed: " << wsOk << std::endl;
		throw wsOk;
	}
	else
	{
		std::cout << "\n[TCPSocketController] ---- LocalHost resolved successfully:\n"
			<< "\t|---- Address: \t" << m_result->ai_addr << "\n"
			<< "\t|---- Family: \t" << m_result->ai_family << "\n"
			<< "\t|---- SckType: \t" << m_result->ai_socktype << "\n"
			<< "\t|---- Protocol: \t" << m_result->ai_protocol << "\n";
	}

	workSocket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);
	if (workSocket == INVALID_SOCKET)
	{
		std::cerr << "\n[TCPSocketController] ---- Error generating socket: " << WSAGetLastError() << std::endl;
		throw - 1;
	}
	
	std::cout << "\n[TCPSocketController] ---- Successfully created Socket Data:" << std::endl;
	std::cout << "\t |---- Socket address:" << workSocket << std::endl;

	return workSocket;
}
