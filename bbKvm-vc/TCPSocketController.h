#pragma once
class addrinfo;
class SOCKET;
class SocketData;

//	Define the buffer size that will be used by the classes that will subclass this one.
#define DEFAULT_BUFLEN 4096

class TCPSocketController
{
public:
	TCPSocketController(char* port, char* address);
	~TCPSocketController();

	SocketData generateSocket(bool isServer);
	char* getPort() { return m_port; };

protected:
	char*	m_port;
	char*	m_address;
};