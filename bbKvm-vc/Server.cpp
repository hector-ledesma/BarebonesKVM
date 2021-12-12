#include "Server.h"
#include <iostream>
#include <string>
#include <sstream>
#include <bitset>

#include "ScreenInfo.h"

#define		BBKVM_BASE			(WM_USER + 0)

#define		BBKVM_MOUSEMOVE		(BBKVM_BASE + 1)
#define		BBKVM_MOUSEWHEEL	(BBKVM_BASE + 2)
#define		BBKVM_XBUTTON		(BBKVM_BASE + 3)
#define		BBKVM_MOUSECLICK	(BBKVM_BASE + 4)

#define		BBKVM_SIMULATEKEY	(BBKVM_BASE + 5)

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
	m_handler.setHooks();

	std::cout << "\n[Server] ---- Running..." << std::endl;

	//	TODO: Refactor entire main loop. We'll use message queues as this is a one-way communcation process.
	char buf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int buflen = DEFAULT_BUFLEN; 
	int err;

	//	Initial handshake
	//do
	//{
	iResult = recv(m_client, buf, buflen, 0);
	if (iResult > 0)
	{
		std::cout << "\n[Server] ---- Bytes received: " << iResult << std::endl;
		std::cout << "\n[Server] ---- Client said: " << std::string(buf, 0, iResult) << std::endl;

		//	For now, just return same message.
		iSendResult = send(m_client, buf, iResult, 0);
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

	//} while (iResult > 0);
	
	std::cout << "\n[Server] ---- Handshake complete. Connection with Client established." << std::endl;
	ScreenInfo::getInstance().lockMouse();

	//	------ Test message loop here -----
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		std::ostringstream ss;

		std::cout << "[Server] ---- message found: " << msg.message << std::endl;
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		std::ostringstream data;
		
		short x, y;
		int flags = msg.lParam;
		switch (msg.message)
		{
		case BBKVM_SIMULATEKEY:
			//	TODO: - Make a create key msg method.
			std::cout << "[Server] ---- Simulating key." << std::endl;
			ss << "\t\t|---Message: " << msg.message << "\r\n" <<
				"\t\t|--wParam: " << msg.wParam << "\r\n"
				"\t\t|--lParam: " << (ULONG)msg.lParam << "\r\n";
			std::cout << ss.str() << std::endl;

			data << msg.message << "[[" << msg.wParam << "[[" << (ULONG)msg.lParam << "[[";
			break;

		case BBKVM_MOUSEMOVE:
			std::cout << "[Server] ---- Simulating mouse movement." << std::endl;

			x = (signed short)HIWORD(msg.wParam);
			y = (signed short)LOWORD(msg.wParam);

			ss << "\t\t|---Move by: " << "(" << x << ", " << y << ")" << "\r\n" <<
				"\t\t|--flags: " << std::bitset<32>(flags) << "\r\n";
			std::cout << ss.str() << std::endl;

			data << msg.message << "[[" << x << "[[" << y << "[[" << msg.lParam << "[[";
			break;

		case BBKVM_MOUSEWHEEL:	
			std::cout << "[Server] ---- Simulating scroll wheel." << std::endl;
			ss << "\t\t|---Mouse delta: " << (short)msg.wParam << "\r\n" <<
				"\t\t|--flags: " << std::bitset<32>(flags) << "\r\n";
			std::cout << ss.str() << std::endl;

			data << msg.message << "[[" << (short)msg.wParam << "[[" << msg.lParam << "[[";
			break;

		case BBKVM_XBUTTON:
			std::cout << "[Server] ---- Simulating x button." << std::endl;
			ss << "\t\t|---XBUTTON: " << (short)msg.wParam << "\r\n" <<
				"\t\t|--flags: " << std::bitset<32>(flags) << "\r\n";
			std::cout << ss.str() << std::endl;

			data << msg.message << "[[" << msg.wParam << "[[" << msg.lParam << "[[";
			break;

		case BBKVM_MOUSECLICK:
			std::cout << "[Server] ---- Simulating click." << std::endl;
			ss << "\t\t|--flags: " << std::bitset<32>(flags) << "\r\n";
			std::cout << ss.str() << std::endl;
			
			data << msg.message << "[[" << msg.wParam << "[[" << msg.lParam << "[[";
			break;

		default:
			std::cout << "[Server] ---- message unaccounted for: " << msg.message << std::endl;
			break;
		}

		std::cout << "[Server] ---- Sending: " << "\n\t|--- " << data.str() << "\n" << std::endl;
		iSendResult = send(m_client, data.str().c_str(), data.str().size() + 1, 0);
		//iSendResult = send(m_client, buf, sizeof(KBDLLHOOKSTRUCT), 0);
		if (iSendResult == SOCKET_ERROR)
		{
			err = WSAGetLastError();
			std::cerr << "\n[Server] ---- Failed to send data: " << err << std::endl;
			throw err;
		}

	}

	//	Tell our client it's time to say adios.
	iResult = shutdown(m_client, SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		std::cerr << "\n[Server] ---- Failed client shutdown(): " << err << std::endl;
		throw err;
	}
}
