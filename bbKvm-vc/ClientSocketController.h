#pragma once
#include "TCPSocketController.h"
#include <WinSock2.h>

class ClientSocketController :	public TCPSocketController
{
public:
	ClientSocketController(const char* port, const char* address) : TCPSocketController(port, address) {};

	SOCKET initServerSocket();
};
