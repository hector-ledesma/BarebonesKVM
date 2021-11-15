#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
//#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

void
server_main() {
	//	Initialize WinSock
	WSADATA wsaData;
	int wsOk = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsOk != 0)
	{
		std::cerr << "WSAStartup failed: " << wsOk << std::endl;
		return;
	} 
	else
	{
		std::cout << "WSAStartup successful" << std::endl;
	}

	//	Resolve Local Address and Port for the server.

	addrinfo hints, *result;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// Stream Socket
	hints.ai_protocol = IPPROTO_TCP;	// TCP protocol
	hints.ai_flags = AI_PASSIVE;		// Caller will user returned socket in a bind() call.

	/*
		getaddrinfo() seems to do some crazy complex DNS lookup related stuff:
		https://jameshfisher.com/2018/02/03/what-does-getaddrinfo-do/

		But in our case, setting the pNodeName param to NULL serves to resolve our local address and port to be used by the server.

		We can then use this information to populate the info necessary for our socket and bind system calls.

		If we already possess all the info we need, we can skip straight to populating a sockaddr_in structure ourselves, as done in Sloan Kelly's tutorials:
		https://www.youtube.com/watch?v=WDn-htpBlnU
		https://stackoverflow.com/questions/23401147/what-is-the-difference-between-struct-addrinfo-and-struct-sockaddr/23401272

		I'll stick to using MSDN's approach for now.
	*/
	wsOk = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (wsOk != 0)
	{
		std::cerr << "getaddrinfo failed: " << wsOk << std::endl;
		WSACleanup();
		return;
	}
	else
	{
		std::cout << "LocalHost resolves successfully successful:\n"
			<< "\t|--- Address: \t" << result->ai_addr << "\n"
			<< "\t|--- Family: \t" << result->ai_family << "\n"
			<< "\t|--- SckType: \t" << result->ai_socktype << "\n"
			<< "\t|--- Protocol: \t" << result->ai_protocol << "\n";
	}

	//	Beg papa Windows for a Socket file descriptor.
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		std::cerr << "Error requesting our listening socket: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	else {
		std::cout << "Successfully created Listening Socket." << std::endl;
	}

	//	Bind our listening socket.
	wsOk = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (wsOk == SOCKET_ERROR)
	{
		std::cerr << "bind() failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	else {
		std::cout << "Successfully bound Listening Socket. Freeing address information." << std::endl;
		freeaddrinfo(result);
	}

	//	Set our bound socket to listen for connections.
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cerr << "listen() call failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	else {
		std::cout << "Listening socket successfully set to listen." << std::endl;
	}

	//	Accept a connecting socket.
	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, nullptr, nullptr);
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cerr << "Failed to accept a client socket: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	else
	{
		std::cout << "Successfully accepted a client socket. Closing listening socket." << std::endl;
		closesocket(ListenSocket);
	}

	//	Main loop

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			std::cout << "Bytes received: " << iResult << std::endl;

			//	For now, just return same message.
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				std::cerr << "Failed to send data: " << WSAGetLastError() << std::endl;
				closesocket(ClientSocket);
				WSACleanup();
				return;
			}
			std::cout << "Bytes sent: " << iSendResult << std::endl;
		}
		else if (iResult == 0)
			std::cout << "No Data received. Closing connection..." << std::endl;
		else
		{
			std::cerr << "recv() failed: " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}

	} while (iResult > 0);

	//	Shut down client socket
	wsOk = shutdown(ClientSocket, SD_SEND);
	if (wsOk == SOCKET_ERROR)
	{
		std::cerr << "shudown() failed: " << WSAGetLastError() << std::endl;
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}


	//	Clean up WinSock
	WSACleanup();
}