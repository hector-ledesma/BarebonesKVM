#include <iostream>
#include <Windows.h>
/*
	This file contains sample code grabbed from:
	https://www.unknowncheats.me/forum/c-and-c-/83707-setwindowshookex-example.html

	Breaking down this code helped me understand the windows Hook system.
*/

//	Not fully sure on HHOOK
HHOOK				_hook2;

LRESULT
CALLBACK	//	CALLBACK is not a keyword. It's simply a preprocessor macro that gets replaced with the appropriate calling convention, which as you already noted is __stdcall.
MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSLLHOOKSTRUCT msStruct = *((MSLLHOOKSTRUCT*)lParam);

	if (wParam == WM_MOUSEMOVE)
	{
		std::cout << "Mouse is moving:" << std::endl;
		std::cout << "\t|---- X:\t" << msStruct.pt.x << std::endl;
		std::cout << "\t|---- Y:\t" << msStruct.pt.y << std::endl;
	}
	if (wParam == WM_LBUTTONDOWN)
	{
		std::cout << "Button down" << std::endl;
		PostQuitMessage(0);
	}

	//return CallNextHookEx(_hook2, nCode, wParam, lParam);
	return 1;
}

void
MouseSetHook()
{
	if (!(_hook2 = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, NULL, 0)))
	{
		MessageBox(
			NULL,
			(LPCWSTR)L"Failed to install hook!",
			(LPCWSTR)L"Error",
			MB_ICONERROR);
	}
}

int
main()
{

	MouseSetHook();

	/*
		https://stackoverflow.com/questions/7458807/why-must-setwindowshookex-be-used-with-a-windows-message-queue
		EXTREMELY IMPORTANT:
		In order for Windows to safely insert the data into your process, it needs to be idle and ready to listen. You do this by calling GetMessage() or PeekMessage(), by having your process insert itself into the message loop, Windows will be able to communicate safely.

		--------
		BIG realization:
		The while loops is NOT looping. It blocks the program so that windows may safely communicate with the process.

		Unless we place messages in THIS SPECIFIC THREAD'S MESSAGE QUEUE, the function will not continue.
	*/
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		std::cout << "msg loop baby: " << msg.message << std::endl;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(_hook2);
}