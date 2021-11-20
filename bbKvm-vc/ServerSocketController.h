#pragma once
#include "TCPSocketController.h"
class SocketData;
class SOCKET;

class ServerSocketController :	public TCPSocketController
{
public:
	ServerSocketController(char* port, char* address) : TCPSocketController(port, address) {};

	SOCKET initListenSocket();
};

