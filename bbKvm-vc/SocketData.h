#pragma once
#include <WinSock2.h>

class SocketData {
public:
	SocketData();
	~SocketData();

	//SOCKET socket();
	void initSocket(const char* port, addrinfo hint);

	addrinfo* getAddr() { return m_result; };
	SOCKET getSocket() { return m_socket; };

private:
	SOCKET		m_socket;
	addrinfo*	m_result;
};