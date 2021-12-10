#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#define		BBKVM_BASE			(WM_USER + 0)

#define		BBKVM_MOUSEMOVE		(BBKVM_BASE + 1)
#define		BBKVM_MOUSEWHEEL	(BBKVM_BASE + 2)
#define		BBKVM_XBUTTON		(BBKVM_BASE + 3)
#define		BBKVM_MOUSECLICK	(BBKVM_BASE + 4)

#define		BBKVM_SIMULATEKEY	(BBKVM_BASE + 5)


void
clientTest_main() {
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
	iResult = getaddrinfo("192.168.0.147", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cerr << "getaddrinfo failed: " << iResult << std::endl;
		WSACleanup();
		return;
	}
	else
	{
		struct sockaddr_in* ipv4 = (struct sockaddr_in*)result->ai_addr;
		char ipstr[INET6_ADDRSTRLEN];
		inet_ntop(result->ai_family, &(ipv4->sin_addr), ipstr, sizeof ipstr);
		std::cout << "LocalHost resolves successfully successful:\n"
			<< "\t|--- Address: \t" << ipstr << "\n"
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
	// Set our result as invalid for starters. We'll use this as one of our loop conditions.
	iResult = INVALID_SOCKET;
	// Testing
	for (addrinfo* p = result; p != nullptr; p = p->ai_next)
	{
		struct sockaddr_in* ipv4 = (struct sockaddr_in*)result->ai_addr;
		char ipstr[INET6_ADDRSTRLEN];
		inet_ntop(result->ai_family, &(ipv4->sin_addr), ipstr, sizeof ipstr);
		std::cout << "\n[TCPSocketController] ---- Testing all addresses:\n"
			<< "\t|----  \t" << ipstr << std::endl;
	}

	do
	{
		//	Attempt to connect to our current address
		/*
			TODO:
			We need to establish some sort of "handshake" between client and server. This way, even if there's another machine with the same port open, we only connect to our program.
		*/
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			std::cerr << "Failed to connect to addr: " << WSAGetLastError() << std::endl;
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)ptr->ai_addr;
			char ipstr[INET6_ADDRSTRLEN];
			inet_ntop(ptr->ai_family, &(ipv4->sin_addr), ipstr, sizeof ipstr);
			std::cout << "Address failed at: " << ipstr << std::endl;
			std::string hello;
			std::cin >> hello;
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
		return;
	}

	int recvbuflen = DEFAULT_BUFLEN;

	const char* sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];

	int loopResult;

	loopResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (loopResult == SOCKET_ERROR) {
		std::cerr << "Failed initial send: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	std::cout << "Handshake complete: " << loopResult << std::endl;
	send(ConnectSocket, nullptr, 0, 0);

	/*loopResult = shutdown(ConnectSocket, SD_SEND);
	if (loopResult == SOCKET_ERROR)
	{
		std::cerr << "Shutdown Failed: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	else {
		std::cout << "Successfully terminated send for ConnectSocket" << std::endl;
	}*/

	//std::string userInput;

	// KBDLLHOOKSTRUCT *kbdStruct;

	do {
		loopResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (loopResult > 0) {
			std::cout << "\nBytes received: " << loopResult << std::endl;

			// kbdStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(recvbuf);

			/*std::cout << "Received:\n"
				<< kbdStruct->scanCode << "\n"
				<< kbdStruct->vkCode << std::endl;*/

			std::string data(recvbuf, 0, loopResult);

			std::cout << "Server said: " << data << std::endl;
			std::string delimiter = "[[";
			auto limit = data.find(delimiter);

			// Here, we break down the data.
			if (limit != std::string::npos)
			{
				std::vector<std::string> recvData;


				int start = 0;
				int end = limit;

				long wParam;
				ULONG flags;
				UINT uSent;
				INPUT input;


				while (end != -1)
				{
					recvData.push_back(data.substr(start, end - start));
					start = end + delimiter.size();
					end = data.find(delimiter, start);
				}

				//	Index 0 will always be our message.
				int id = std::stoi(recvData.at(0));
				std::cout << "Extracted ID: " << id << std::endl;
				switch (id)
				{
				case BBKVM_SIMULATEKEY:
					wParam = std::stol(recvData.at(1), nullptr, 0);
					flags = std::stoul(recvData.at(2), nullptr, 0);

					std::cout << "Simulated key vkCode: " << wParam << std::endl;
					std::cout << "Simulated key flags: " << std::bitset<32>(flags) << std::endl;

					// Here, we simulate input.
					input.type = INPUT_KEYBOARD;
					input.ki.wVk = wParam;
					input.ki.wScan = 0;
					input.ki.dwFlags = 0;

					if (flags & (1lu << 31))
					{
						std::cout << "Releasing key." << std::endl;
						input.ki.dwFlags |= KEYEVENTF_KEYUP;
					}
					else
					{
						std::cout << "Pressing key" << std::endl;
					}
					uSent = SendInput(1, &input, sizeof(INPUT));
					if (uSent != 1)
					{
						std::cerr << "SendInput Failed :" << HRESULT_FROM_WIN32(GetLastError()) << std::endl;
					}
					break;
				case BBKVM_MOUSEMOVE:
					std::cout << "Moving x: " << std::stoi(recvData.at(1)) << std::endl;
					std::cout << "Moving y: " << std::stoi(recvData.at(2)) << std::endl;
					std::cout << "flags: " << std::bitset<32>(std::stoi(recvData.at(3))) << std::endl;
					input.type = INPUT_MOUSE;
					input.mi.dx = std::stoi(recvData.at(2)) * -1;
					input.mi.dy = std::stoi(recvData.at(1)) * -1;
					input.mi.dwFlags = std::stoi(recvData.at(3));
					input.mi.mouseData = 0;
					input.mi.time = 0;
					uSent = SendInput(1, &input, sizeof(INPUT));
					if (uSent != 1)
					{
						std::cerr << "SendInput Failed :" << GetLastError() << std::endl;
					}
					break;
				case BBKVM_MOUSEWHEEL:
					std::cout << "Scrolling : " << (std::stoi(recvData.at(1)) > 0 ? "UP" : "DOWN") << " " << std::stoi(recvData.at(1)) << std::endl;
					std::cout << "flags: " << std::bitset<32>(std::stoi(recvData.at(2))) << std::endl;
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = std::stoi(recvData.at(2));
					input.mi.mouseData = std::stoi(recvData.at(1));
					input.mi.time = 0;
					uSent = SendInput(1, &input, sizeof(INPUT));
					if (uSent != 1)
					{
						std::cerr << "SendInput Failed :" << GetLastError() << std::endl;
					}
					break;
				case BBKVM_MOUSECLICK:
					std::cout << "Clicking." << std::endl;
					std::cout << "flags: " << std::bitset<32>(std::stoi(recvData.at(2))) << std::endl;
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = std::stoi(recvData.at(2));
					input.mi.mouseData = 0;
					input.mi.time = 0;
					uSent = SendInput(1, &input, sizeof(INPUT));
					if (uSent != 1)
					{
						std::cerr << "SendInput Failed :" << GetLastError() << std::endl;
					}
					break;
				case BBKVM_XBUTTON:
					std::cout << "Xbutton" << std::endl;
					std::cout << "flags: " << std::bitset<32>(std::stoi(recvData.at(2))) << std::endl;
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = std::stoi(recvData.at(2));
					input.mi.mouseData = std::stoi(recvData.at(1));
					input.mi.time = 0;
					uSent = SendInput(1, &input, sizeof(INPUT));
					if (uSent != 1)
					{
						std::cerr << "SendInput Failed :" << GetLastError() << std::endl;
					}
					break;
				default:
					std::cout << "Unrecognizable id: " << id << std::endl;
					break;
				}

			}

		}
		else if (loopResult == 0)
			std::cout << "Connection closed." << std::endl;
		else
			std::cout << "recv() failed: " << WSAGetLastError() << std::endl;

		/*std::cout << ">\t";
		std::getline(std::cin, userInput);

		if (userInput.size() == 0)
			loopResult = send(ConnectSocket, nullptr, 0, 0);
		else*/
		//loopResult = send(ConnectSocket, userInput.c_str(), userInput.size() + 1, 0);


	} while (loopResult > 0);

	//	Clean up after we're done
	loopResult = shutdown(ConnectSocket, SD_BOTH);
	if (loopResult == SOCKET_ERROR)
	{
		std::cerr << "Shutdown Failed: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	else {
		std::cout << "Successfully terminated send for ConnectSocket" << std::endl;
	}
	closesocket(ConnectSocket);
	WSACleanup();
}