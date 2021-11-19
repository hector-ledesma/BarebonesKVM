#pragma once
class SOCKET;
class addrinfo;

class SocketData {
public:
	SocketData(TCPSocketController* controller, addrinfo hint);
	~SocketData();

	//SOCKET socket();
	void initSocket();
	bool getaddrSuccessful() { return m_result != nullptr; };


	addrinfo* addr();
	bool setAddr(addrinfo* addr);
private:
	SOCKET		m_socket;
	addrinfo*	m_result;
};