#include "Server.h"
#include <iostream>

Server::Server() 
	:	m_controller(ServerSocketController(DEFAULT_PORT, DEFAULT_HOST)), 
		m_client(m_controller.initListenSocket()) 
{
	std::cout << "\n[Server] ---- Constructor called." << std::endl;

};

Server::~Server() 
{
	std::cout << "\n[Server] ---- Destructor called." << std::endl;
	closesocket(m_listener);
	closesocket(m_client);
}

void 
Server::getClient()
{
	std::cout << "\n[Server] ---- Waiting for Client..." << std::endl;
	m_client = accept(m_listener, nullptr, nullptr);

	if (m_client == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		std::cerr << "\n[Server] ---- Failed to accept a client socket: " << err << std::endl;
		throw err;
	}
}

void
Server::run()
{
	std::cout << "\n[Server] ---- Running..." << std::endl;

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	int err;
	do 
	{
		iResult = recv(m_client, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			std::cout << "\n[Server] ---- Bytes received: " << iResult << std::endl;

			std::cout << "\n[Server] ---- Client said: " << std::string(recvbuf, 0, iResult) << std::endl;

			//	For now, just return same message.
			iSendResult = send(m_client, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				err = WSAGetLastError();
				std::cerr << "\n[Server] ---- Failed to send data: " << err << std::endl;
				throw err;
			}
			std::cout << "\n[Server] ---- Bytes sent: " << iSendResult << std::endl;
		}
		else if (iResult == 0)
			std::cout << "\n[Server] ---- No Data received. Closing connection..." << std::endl;
		else
		{
			err = WSAGetLastError();
			std::cerr << "\n[Server] ---- recv() failed: " << err << std::endl;
			throw err;
		}

	} while (iResult > 0);

	iResult = shutdown(m_client, SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		std::cerr << "\n[Server] ---- Failed client shutdown(): " << err << std::endl;
		throw err;
	}
}
