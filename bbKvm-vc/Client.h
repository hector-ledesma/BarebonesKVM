#pragma once

#include "ClientSocketController.h"

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 4096

class Client
{
public:
	Client();
	~Client();

	void run();
private:

	void findServer();

	ClientSocketController m_controller;

	SOCKET m_serverSocket;
};
