#pragma once
#include <WinSock2.h>
#include <iostream>

class SocketData {
public:
	SocketData();
	SocketData(const SocketData& sd1) 
	{
		std::cout << "\n[SocketData] ---- Copy Constructor called." << std::endl;
		this->m_socket = sd1.m_socket;
		this->m_result = sd1.m_result;
	};
	~SocketData();

	//SOCKET socket();
	void initSocket(const char* port, addrinfo hint);

	addrinfo* getAddr() { return m_result; };
	SOCKET getSocket() { return m_socket; };

private:
	SOCKET		m_socket;
	addrinfo*	m_result;
};