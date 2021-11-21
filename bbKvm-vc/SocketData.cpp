#include "SocketData.h"

#include <iostream>
#include <WS2tcpip.h>

SocketData::SocketData() : m_result(nullptr), m_socket(INVALID_SOCKET) {
	std::cout << "\n[SocketData] ---- Constructor called." << std::endl;

}

//	This class will take care of cleaning up addrinfo and its associated socket.
SocketData::~SocketData() {
	std::cout << "\n[SocketData] ---- Destructor called." << std::endl;

	if (m_result != nullptr)
	{
		std::cout << "\t |---- Freeing addrinfo." << std::endl;
		freeaddrinfo(m_result);
	}
	
	std::cout << "\t |---- Closing socket." << std::endl;
	closesocket(m_socket);
}


void SocketData::initSocket(const char* port, addrinfo hint) {

	int wsOk = getaddrinfo(NULL, port, &hint, &m_result);
	if (wsOk != 0)
	{
		std::cerr << "\n[SocketData] ---- geraddrinfo failed: " << wsOk << std::endl;
		throw wsOk;
	}
	else
	{
		std::cout << "\n[SocketData] ---- LocalHost resolved successfully:\n"
			<< "\t|---- Address: \t" << m_result->ai_addr << "\n"
			<< "\t|---- Family: \t" << m_result->ai_family << "\n"
			<< "\t|---- SckType: \t" << m_result->ai_socktype << "\n"
			<< "\t|---- Protocol: \t" << m_result->ai_protocol << "\n";
	}

	m_socket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);
	if (m_socket == INVALID_SOCKET)
	{
		std::cerr << "Error generating socket: " << WSAGetLastError() << std::endl;
		throw - 1;
	}
	std::cout << "Successfully created initialized Socket through SocketData." << std::endl;
}