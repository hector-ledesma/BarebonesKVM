#pragma once
#include <WinSock2.h>

class SocketData;

/*
	Generating a socket is a pretty universal procedure, outside of the passive flag.

	This class can be subclassed to generate server or client specific ports/functionality.
*/
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
	SOCKET generateSocket(bool isServer = false);
	const char* getPort();

protected:
	const char*	m_port;
	const char*	m_address;
	/*
		Creating and using a socket relies on information from our addrinfo.
		We store it in our controller so that it may be used by any instance and/or children of this class.
	*/
	addrinfo*	m_result;
};