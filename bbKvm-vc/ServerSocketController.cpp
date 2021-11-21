#include "ServerSocketController.h"
#include "SocketData.h"
#include <iostream>


ServerSocketController::ServerSocketController(const char* port, const char* address) 
	: TCPSocketController(port, address) 
{
	std::cout << "\n[ServerSocketController] ---- Constructor called." << std::endl;
};

ServerSocketController::~ServerSocketController()
{
	std::cout << "\n[ServerSocketController] ---- Destructor called." << std::endl;

}
SOCKET ServerSocketController::initListenSocket() {
	std::cout << "\n[ServerSocketController] ---- Initializing a Listening Socket." << std::endl;

	SocketData socketContainer = generateSocket(true);
	addrinfo* result = socketContainer.getAddr();
	SOCKET listenSocket = socketContainer.getSocket();

	std::cout << "\n[ServerSocketController] ---- Calling bind()." << std::endl;
	int iResult;
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		std::cerr << "\n[ServerSocketController] ---- bind() failed with error: " << err << std::endl;
		throw err;
	}

	std::cout << "\n[ServerSocketController] ---- Calling listen()." << std::endl;
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		std::cerr << "\n[ServerSocketController] ---- bind() failed with error: " << err << std::endl;
		throw err;
	}

	return listenSocket;
}