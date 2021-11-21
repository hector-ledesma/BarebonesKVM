#include "ServerSocketController.h"
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
SOCKET ServerSocketController::initListenSocket() 
{
	std::cout << "\n[ServerSocketController] ---- Initializing a Listening Socket." << std::endl;

	SOCKET listenSocket = generateSocket(true);
	std::cout << "\n[ServerSocketController] ---- Storing SocketData information." << std::endl;
	//addrinfo* result = socketContainer.getAddr();
	//SOCKET listenSocket = socketContainer.getSocket();

	std::cout << "\n[ServerSocketController] ---- Calling bind()." << std::endl;
	int iResult;
	iResult = bind(listenSocket, m_result->ai_addr, (int)m_result->ai_addrlen);
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