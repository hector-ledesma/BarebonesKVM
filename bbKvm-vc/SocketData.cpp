#include "TCPSocketController.h"
#include "SocketData.h"


#include <WS2tcpip.h>
#include <iostream>
 
SocketData::SocketData(TCPSocketController* controller, addrinfo hint) : m_socket(INVALID_SOCKET) {

	int wsOk = getaddrinfo(NULL, controller->getPort(), &hint, &m_result);
	if (wsOk != 0)
	{
		std::cerr << "geraddrinfo failed: " << wsOk << std::endl;
		throw wsOk;
	}
	else
	{
		std::cout << "LocalHost resolved successfully:\n"
			<< "\t|--- Address: \t" << m_result->ai_addr << "\n"
			<< "\t|--- Family: \t" << m_result->ai_family << "\n"
			<< "\t|--- SckType: \t" << m_result->ai_socktype << "\n"
			<< "\t|--- Protocol: \t" << m_result->ai_protocol << "\n";
	}
}

//	This class will tak ecare of cleaning up addrinfo and its associated socket.
SocketData::~SocketData() {
	if (m_result != nullptr)
		freeaddrinfo(m_result);
	
	closesocket(m_socket);
}


void SocketData::initSocket() {
	m_socket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);
	if (m_socket == INVALID_SOCKET)
	{
		std::cerr << "Error generating socket: " << WSAGetLastError() << std::endl;
		throw - 1;
	}
	std::cout << "Successfully created initialized Socket through SocketData." << std::endl;
}