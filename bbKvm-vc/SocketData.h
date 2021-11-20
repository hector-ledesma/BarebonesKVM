#pragma once
class SOCKET;
class addrinfo;

class SocketData {
public:
	SocketData();
	~SocketData();

	//SOCKET socket();
	void initSocket(TCPSocketController* controller, addrinfo hint);
	bool getaddrSuccessful() { return m_result != nullptr; };


	addrinfo* getAddr() { return m_result; };
	SOCKET getSocket() { return m_socket; };

	bool setAddr(addrinfo* addr);
private:
	SOCKET		m_socket;
	addrinfo*	m_result;
};