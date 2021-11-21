#include "Server.h"
#include <iostream>

Server::Server() 
	:	m_controller(ServerSocketController(DEFAULT_PORT, DEFAULT_HOST)), 
		m_client(m_controller.initListenSocket()) 
{};

Server::~Server() 
{
	closesocket(m_listener);
	closesocket(m_client);
}

void 
Server::getClient()
{
	std::cout << "\nWaiting for Client..." << std::endl;
	m_client = accept(m_listener, nullptr, nullptr);

	if (m_client == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		std::cerr << "Failed to accept a client socket: " << err << std::endl;
		throw err;
	}
}

void
Server::run()
{
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	int err;
	do 
	{
		iResult = recv(m_client, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			std::cout << "\nBytes received: " << iResult << std::endl;

			std::cout << "Client said: " << std::string(recvbuf, 0, iResult) << std::endl;

			//	For now, just return same message.
			iSendResult = send(m_client, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				err = WSAGetLastError();
				std::cerr << "Failed to send data: " << err << std::endl;
				throw err;
			}
			std::cout << "\nBytes sent: " << iSendResult << std::endl;
		}
		else if (iResult == 0)
			std::cout << "No Data received. Closing connection..." << std::endl;
		else
		{
			err = WSAGetLastError();
			std::cerr << "recv() failed: " << err << std::endl;
			throw err;
		}

	} while (iResult > 0);

	iResult = shutdown(m_client, SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		std::cerr << "Failed client shutdown(): " << err << std::endl;
		throw err;
	}
}
