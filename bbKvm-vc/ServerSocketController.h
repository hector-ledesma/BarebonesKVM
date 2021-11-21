#pragma once
#include <WinSock2.h>
#include "TCPSocketController.h"


class ServerSocketController :	public TCPSocketController
{
public:
	ServerSocketController(const char* port, const char* address);

	SOCKET initListenSocket();
};

