#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"


void 
main() {
	WSADATA wsaData;
	int wsOk = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsOk != 0)
	{
		std::cerr << "WSAStartup failed: " << wsOk << std::endl;
	}
	else
	{
		std::cout << "WSAStartup successful" << std::endl;
	}

	//	Set up our addrinfo structure to request server information.
	addrinfo hints, 
		* result;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			// MS Docs use AF_UNSPEC, I'm certain I want ot use IPv4 so I'll go with AF_INET.
	hints.ai_socktype = SOCK_STREAM;	// Streaming socket
	hints.ai_protocol = IPPROTO_TCP;	// TCP/IP protocol

	//	Resolve the server address and port.
	int iResult;
	/*
		I'll be developing the program using the same host, or another local machine I have full control of, so for the first param of geaddrinfo() I can specify 'localhost' or my other machine's ip.

		TODO:
		Figure out how to  find a server within your local network without knowing its domain name lol.
	*/
	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cerr << "getaddrinfo failed: " << iResult << std::endl;
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

	//	Create connection socket
	SOCKET ConnectSocket = INVALID_SOCKET;
	addrinfo* ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET)
	{
		std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	else
	{
		std::cout << "Successfully created Connection socket." << std::endl;
	}

	//	Connect to a socket/server
	/*
		As it turns out, getaddrinfo() fills our result pointer with a linked list of all addresses that matched our criteria. 

		Therefore, it is up to us to attempt to connect to all those addresses and establish our connection with the correct one.

		I believe I can achieve this by looping here until we find the correct address.

		If we're down here, we've successfully received at least one addrinfo structure. We also have a connect socket, which we don't have to re-create, as we only specified characteristics for the connection, and not specifics for the server
	*/
	// Set our result as invalid for starters. We'll use this as one of our loop arguments.
	iResult = INVALID_SOCKET;

	do
	{
		//	Attempt to connect to our current address
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
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

	//	At this point, we either found our server, or we're ready to exit early.
	//	Either way, we can clean up our result memory.
	freeaddrinfo(result);

	//	If we exit our loop, and our result is invalid and our pointer is null, we didn't find a valid address.
	if (iResult == INVALID_SOCKET && ptr == nullptr)
	{
		std::cerr << "Failed to find server!. Exiting early." << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
	}

	//	Clean up after we're done
	freeaddrinfo(result);
	WSACleanup();
}