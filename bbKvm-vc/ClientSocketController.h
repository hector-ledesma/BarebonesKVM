#pragma once
#include "TCPSocketController.h"

class SocketData;
class SOCKET;

class ClientSocketController :	public TCPSocketController
{
public:
	ClientSocketController(char* port, char* address) : TCPSocketController(port, address) {};

	SOCKET initServerSocket();
};
