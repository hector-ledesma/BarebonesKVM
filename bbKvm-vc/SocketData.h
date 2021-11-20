#pragma once
class SOCKET;
class addrinfo;

class SocketData {
public:
	SocketData();
	~SocketData();

	//SOCKET socket();
	void initSocket(TCPSocketController* controller, addrinfo hint);

	addrinfo* getAddr() { return m_result; };
	SOCKET getSocket() { return m_socket; };

private:
	SOCKET		m_socket;
	addrinfo*	m_result;
};