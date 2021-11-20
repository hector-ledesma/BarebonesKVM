#include "ClientSocketController.h"
#include "SocketData.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

SOCKET 
ClientSocketController::initServerSocket()
{
	SocketData socketContainer = generateSocket();
	SOCKET serverSocket = socketContainer.getSocket();
	addrinfo* result = socketContainer.getAddr();
	addrinfo* ptr = result;

	int iResult = INVALID_SOCKET;

	do
	{
		//	Attempt to connect to our current address
		/*
			TODO:
			We need to establish some sort of "handshake" between client and server. This way, even if there's another machine with the same port open, we only connect to our program.
		*/
		iResult = connect(serverSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			std::cerr << "Failed to connect to addr: " << WSAGetLastError() << std::endl;
			/*
				If we fail, we want to try with the next address on the list so:
				|-- set iResult to invalid, so we may check once we exit the loop.
				|-- move our ptr to the next. We keep our result intact in case we ever need to start over/reference earlier addresses.

			*/
			iResult = INVALID_SOCKET;
			ptr = ptr->ai_next;
		}


	} while (iResult == INVALID_SOCKET && ptr != nullptr);

	//	If we exit our loop, and our result is invalid and our pointer is null, we didn't find a valid address.
	if (iResult == INVALID_SOCKET && ptr == nullptr)
	{
		std::cerr << "Failed to find server!. Exiting early." << std::endl;
		throw - 1;
	}

	return serverSocket;
}