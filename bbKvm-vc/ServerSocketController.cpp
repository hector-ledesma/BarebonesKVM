#include "ServerSocketController.h"
#include "SocketData.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>


SOCKET ServerSocketController::initListenSocket() {

	SocketData socketContainer = generateSocket(true);
	addrinfo* result = socketContainer.getAddr();
	SOCKET listenSocket = socketContainer.getSocket();

	int iResult;
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		std::cerr << "bind() failed with error: " << err << std::endl;
		throw err;
	}

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		std::cerr << "bind() failed with error: " << err << std::endl;
		throw err;
	}

	return listenSocket;
}