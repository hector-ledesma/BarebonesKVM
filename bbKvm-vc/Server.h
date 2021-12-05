#pragma once
#include <WinSock2.h>
#include "ServerSocketController.h"
#include "HooksHandler.h"
#include "ScreenInfo.h"

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 4096

/*
	This class is where all actual functionality would be implemented as it pertains to the host machine.
	Things like handling settings, user input, etc.
*/
class Server
{
public:
	Server();
	~Server();
	

	//	Main network/server loop.
	void run();

	

private:

	//	Used for validation.
	bool haveClient();

	/*
		This function will only look for, and store a single socket connection.

		If we wanted to handle multiple clients, this method would be restructured to append to our container of client sockets.

		This functionality should run in separate threads, away from public methods and use the message queue to sync up with the main thread.
	*/
	void getClient();

	/*
		Our controller will allow us to make use of our TCP methods.
	*/
	ServerSocketController	m_controller;

	/*
		Our listener socket that the server will rely on to handle network communication.
		As it is specific and unique to our server, it's here instead of in our Controller classes.
	*/
	SOCKET					m_listener;

	/*
		For the purpose of this project, we only want to handle one single connection.

		If we wanted to handle multiple clients, we could create some form of class/way to identify sockets for each connection, and store the sockets in a container/list.
	*/
	SOCKET					m_client;

	HooksHandler			m_handler;
	
	ScreenInfo				m_screen;
};

