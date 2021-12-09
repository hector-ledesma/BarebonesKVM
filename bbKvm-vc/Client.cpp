#include "Client.h"
#include <iostream>


#define		BBKVM_BASE			(WM_USER + 0)

#define		BBKVM_MOUSEMOVE		(BBKVM_BASE + 1)
#define		BBKVM_MOUSEWHEEL	(BBKVM_BASE + 2)
#define		BBKVM_XBUTTON		(BBKVM_BASE + 3)
#define		BBKVM_MOUSECLICK	(BBKVM_BASE + 4)

#define		BBKVM_SIMULATEKEY	(BBKVM_BASE + 5)


Client::Client()
	:	m_controller(ClientSocketController(DEFAULT_PORT, DEFAULT_HOST)),
		m_serverSocket(INVALID_SOCKET)
{
	std::cout << "\n[Client] ---- Constructor called." << std::endl;
}

Client::~Client() 
{
	closesocket(m_serverSocket);
}

void
Client::findServer()
{
	std::cout << "\n[Client] ---- Waiting for Server..." << std::endl;

	m_serverSocket = m_controller.generateSocket();

	if (m_serverSocket == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		std::cerr << "\n[Client] ---- Failed to connect to server: " << err << std::endl;
		throw err;
	}

}

void
keyboardEvent() {}

void mouseEvent() {}

void
Client::run()
{
	const char* probe = "Initial ping from client to server.";

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, err;

	iResult = send(m_serverSocket, probe, (int)strlen(probe), 0);
	if (iResult == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		std::cerr << "\n[Client] ---- Failed initial probe to server: " << err << std::endl;
		throw err;
	}
	std::cout << "\n[Client] ---- Initial handshake successful." << std::endl;

	do
	{
		iResult = recv(m_serverSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0)
		{
			std::cout << "\n[Client] ---- Bytes received: " << iResult << std::endl;

			std::string data(recvbuf, 0, iResult);

			std::cout << "\n[Client] ---- Data from server: \t" << data << std::endl;


		} 
		else if (iResult == 0)
			std::cout << "\n[Client] ---- Connection closed." << std::endl;
		else
		{
			err = WSAGetLastError();
			std::cout << "\n[Client] ---- recv() failed: " << std::endl;
			throw err;
		}


	} while (iResult > 0);

	iResult = shutdown(m_serverSocket, SD_BOTH);

	if (iResult == SOCKET_ERROR)
		std::cerr << "\n[Client] ---- Socket shutdown failure." << std::endl;

	else
		std::cout << "\n[Client] ---- Socket shutdown successful." << std::endl;

}