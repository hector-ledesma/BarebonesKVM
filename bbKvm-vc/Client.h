#pragma once

#include "ClientSocketController.h"

//#define DEFAULT_SERVER "192.168.0.147"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 4096

class Client
{
public:
	Client(const char* ip);
	~Client();

	void run();
private:

	void findServer();

	ClientSocketController m_controller;

	SOCKET m_serverSocket;

	const char* m_ip;
};
