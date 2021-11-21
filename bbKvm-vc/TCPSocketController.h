#pragma once

class SocketData;

class TCPSocketController
{
public:
	TCPSocketController( const char* port, const char* address);
	~TCPSocketController();

	/*
		This method accepts a boolean value that determines whether the socket generated will contain the ai_flag = AI_PASSIVE.
		Set to true if the generated socket will be used in a bind() call.
		Default value = false.
	*/
	SocketData generateSocket(bool isServer = false);
	const char* getPort();

protected:
	const char*	m_port;
	const char*	m_address;
};