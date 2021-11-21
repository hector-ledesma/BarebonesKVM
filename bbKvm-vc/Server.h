#pragma once
#include <WinSock2.h>
#include "ServerSocketController.h"

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 4096

class Server
{
public:
	Server();
	~Server();

	void getClient();
	void run();

private:
	ServerSocketController	m_controller;
	SOCKET					m_listener;
	SOCKET					m_client;
};

