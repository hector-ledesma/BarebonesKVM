#include "Client.h"
#include <iostream>
#include <vector>
#include <string>
#include <bitset>


#define		BBKVM_BASE			(WM_USER + 0)

#define		BBKVM_MOUSEMOVE		(BBKVM_BASE + 1)
#define		BBKVM_MOUSEWHEEL	(BBKVM_BASE + 2)
#define		BBKVM_XBUTTON		(BBKVM_BASE + 3)
#define		BBKVM_MOUSECLICK	(BBKVM_BASE + 4)

#define		BBKVM_SIMULATEKEY	(BBKVM_BASE + 5)

#define		DATA_DELIMITER		"[["


Client::Client(const char* ip)
	:	m_ip(ip),
		m_controller(ClientSocketController(DEFAULT_PORT, ip)),
		m_serverSocket(m_controller.initServerSocket())
{
	std::cout << "\n[Client] ---- Constructor called." << std::endl;
}

Client::~Client() 
{
	closesocket(m_serverSocket);
}

void
Client::findServer()
{
	std::cout << "\n[Client] ---- Waiting for Server..." << std::endl;

	m_serverSocket = m_controller.generateSocket();

	if (m_serverSocket == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		std::cerr << "\n[Client] ---- Failed to connect to server: " << err << std::endl;
		throw err;
	}

}

int
keyboardEvent(std::vector<std::string>& cmds, INPUT& input) 
{
	long wParam = std::stol(cmds.at(1), nullptr, 0);
	ULONG flags = std::stoul(cmds.at(2), nullptr, 0);

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
	return SendInput(1, &input, sizeof(INPUT));
}

int 
mouseEvent(std::vector<std::string>& cmds, INPUT& input)
{
	int id = std::stoi(cmds.at(0));
	switch (id)
	{
	case BBKVM_MOUSEMOVE:
		std::cout << "\n[Client] ---- Moving mouse:" << std::endl;
		std::cout << "\t\t|-- x: " << std::stoi(cmds.at(2)) << std::endl;
		std::cout << "\t\t|-- y: " << std::stoi(cmds.at(1)) << std::endl;
		std::cout << "\t\t|-- flags: " << std::bitset<32>(std::stoi(cmds.at(3))) << std::endl;

		input.type = INPUT_MOUSE;
		input.mi.dx = std::stoi(cmds.at(2)) * -1;
		input.mi.dy = std::stoi(cmds.at(1)) * -1;
		input.mi.dwFlags = std::stoi(cmds.at(3));
		input.mi.mouseData = 0;
		input.mi.time = 0;
		break;

	case BBKVM_MOUSEWHEEL:
		std::cout << "\n[Client] ---- Scrolling : " << (std::stoi(cmds.at(1)) > 0 ? "UP" : "DOWN") << " " << std::stoi(cmds.at(1)) << std::endl;
		std::cout << "\t\t|-- flags: " << std::bitset<32>(std::stoi(cmds.at(2))) << std::endl;
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = std::stoi(cmds.at(2));
		input.mi.mouseData = std::stoi(cmds.at(1));
		input.mi.time = 0;
		break;

	case BBKVM_MOUSECLICK:
		std::cout << "\n[Client] ---- Simulating  clicking." << std::endl;
		std::cout << "\t\t|-- flags: " << std::bitset<32>(std::stoi(cmds.at(2))) << std::endl;
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = std::stoi(cmds.at(2));
		input.mi.mouseData = 0;
		input.mi.time = 0;
		break;

	case BBKVM_XBUTTON:
		std::cout << "\n[Client] ---- Simulating Xbutton" << std::endl;
		std::cout << "\t\t|-- flags: " << std::bitset<32>(std::stoi(cmds.at(2))) << std::endl;
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = std::stoi(cmds.at(2));
		input.mi.mouseData = std::stoi(cmds.at(1));
		input.mi.time = 0;
		break;

	default:
		std::cout << "\n[Client | MouseEventFunc] ---- Data was passed, but id is not recognized: " << id << std::endl;
		break;
	}

	return SendInput(1, &input, sizeof(INPUT));

}

std::vector<std::string>
splitData(std::string& data)
{
	std::vector<std::string> dat;
	std::string delimiter = DATA_DELIMITER;
	auto limit = data.find(delimiter);
	if (limit != std::string::npos)
	{
		int start = 0;
		int end = limit;

		while (end != -1)
		{
			dat.push_back(data.substr(start, end - start));
			start = end + delimiter.size();
			end = data.find(delimiter, start);
		}
	}
	else
		std::cerr << "[Client | SplitDataFunc] ---- Data didn't contain delimiter." << std::endl;
	

	return dat;
}

void
Client::run()
{
	const char* probe = "Initial ping from client to server.";

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, err;

	// Initial probe/handshake makes sure we have the right server.
	iResult = send(m_serverSocket, probe, (int)strlen(probe), 0);
	if (iResult == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		std::cerr << "\n[Client] ---- Failed initial probe to server: " << err << std::endl;
		throw err;
	}
	iResult = recv(m_serverSocket, recvbuf, DEFAULT_BUFLEN, 0);
	if (iResult > 0 && probe == recvbuf)
		std::cout << "\n[Client] ---- Initial handshake successful." << std::endl;

	do
	{
		iResult = recv(m_serverSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0)
		{
			std::cout << "\n[Client] ---- Bytes received: " << iResult << std::endl;

			std::string data(recvbuf, 0, iResult);

			std::cout << "\n[Client] ---- Data from server: \t" << data << std::endl;

			auto cmds = splitData(data);

			if (cmds.size() == 0)
			{
				std::cout << "\n[Client] ---- Data couldn't be split." << std::endl;
				continue;
			}

			int id = std::stoi(cmds.at(0));
			INPUT input;
			switch (id)
			{
			case BBKVM_SIMULATEKEY:
				if (keyboardEvent(cmds, input) != 1)
				{
					err = HRESULT_FROM_WIN32(GetLastError());
					std::cerr << "\n[Client] ---- Keyboard SendInput Failed :" << err << std::endl;
				}
				break;

			case BBKVM_MOUSEMOVE:
			case BBKVM_MOUSEWHEEL:
			case BBKVM_MOUSECLICK:
			case BBKVM_XBUTTON:
				if (mouseEvent(cmds, input) != 1)
				{
					err = HRESULT_FROM_WIN32(GetLastError());
					std::cerr << "\n[Client] ---- Mouse SendInput Failed :" << err << std::endl;
				}
				break;

			default:
				std::cout << "\n[Client] ---- Unrecognizable id : " << id << std::endl;
				break;
			}

		} 
		else if (iResult == 0)
		{
			std::cout << "\n[Client] ---- Connection closed." << std::endl;
		}
		else
		{
			err = WSAGetLastError();
			std::cout << "\n[Client] ---- recv() failed: " << std::endl;
			throw err;
		}


	} while (iResult > 0);

	iResult = shutdown(m_serverSocket, SD_BOTH);

	if (iResult == SOCKET_ERROR)
		std::cerr << "\n[Client] ---- Socket shutdown failure." << std::endl;

	else
		std::cout << "\n[Client] ---- Socket shutdown successful." << std::endl;

}