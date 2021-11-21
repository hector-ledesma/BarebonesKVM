#pragma once
#include <WinSock2.h>
#include "TCPSocketController.h"


class ServerSocketController :	public TCPSocketController
{
public:
	ServerSocketController(const char* port, const char* address);
	~ServerSocketController();

	/*
		The first step where using a socket for a client and server differs, is in binding the socket to a port and address so that we may listen for client connection requests.

		This method will use a generated socket from the TCPSocketController parent class and return a bound socket we can listen on.
	*/
	SOCKET initListenSocket();
};

