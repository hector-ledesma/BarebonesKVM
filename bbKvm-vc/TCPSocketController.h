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

	/*
		This method accepts a boolean value that determines whether the socket generated will contain the ai_flag = AI_PASSIVE.
		Set to true if the generated socket will be used in a bind() call.
		Default value = false.
	*/
	SocketData generateSocket(bool isServer = false);
	char* getPort() { return m_port; };

protected:
	char*	m_port;
	char*	m_address;
};