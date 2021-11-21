#include "Server.h"
#include <iostream>

/*
	The Server constructor will first initialize our controller using our defined port and host in Server.h.

	Then, use our controller to get our listening socket.

	We default our client to INVALID_SOCKET, as finding the client socket is specific to how our server will implement the handshake.
*/
Server::Server() 
	:	m_controller(ServerSocketController(DEFAULT_PORT, DEFAULT_HOST)),
		m_listener(m_controller.initListenSocket()),
		m_client(INVALID_SOCKET) 
{
	std::cout << "\n[Server] ---- Constructor called." << std::endl;

};

/*
	We want to make sure we close sockets that we use.

	If we wanted to, we could close the listener socket as soon as our client socket becomes available.
*/
Server::~Server() 
{
	std::cout << "\n[Server] ---- Destructor called. Closing listening and client sockets." << std::endl;
	closesocket(m_listener);
	closesocket(m_client);
}

bool
Server::haveClient()
{
	return m_client != INVALID_SOCKET;
}

void 
Server::getClient()
{
	std::cout << "\n[Server] ---- Waiting for Client..." << std::endl;
	/*
		We only make it this far into the program if we have a valid listening socket.

		Therefore, we can use it to create our client socket.
	*/
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
	// If we don't have a client yet, stop until we receive a connection request and successfully initialize our client socket.
	if (!haveClient()) getClient();

	std::cout << "\n[Server] ---- Running..." << std::endl;

	//	TODO: Refactor entire main loop. We'll use message queues as this is a one-way communcation process.
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

	//	Tell our client it's time to say adios.
	iResult = shutdown(m_client, SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		std::cerr << "\n[Server] ---- Failed client shutdown(): " << err << std::endl;
		throw err;
	}
}
