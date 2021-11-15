#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

void client_main() {
	WSADATA wsaData;
	int wsOk = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsOk != 0)
	{
		std::cout << "WSAStartup failed: " << wsOk << std::endl;
	}

	WSACleanup();
}