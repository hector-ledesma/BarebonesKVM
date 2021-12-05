#include <iostream>
#include <Windows.h>
/*
	This file contains sample code grabbed from:
	https://www.unknowncheats.me/forum/c-and-c-/83707-setwindowshookex-example.html

	Breaking down this code helped me understand the windows Hook system.
*/

//	Not fully sure on HHOOK
HHOOK				_hook2;

//	Global point variable where we store mouse position
POINT mPos;

LRESULT
CALLBACK	//	CALLBACK is not a keyword. It's simply a preprocessor macro that gets replaced with the appropriate calling convention, which as you already noted is __stdcall.
MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode == HC_ACTION)
	{
		MSLLHOOKSTRUCT msStruct = *((MSLLHOOKSTRUCT*)lParam);

		/*
			When looking at the definition of WM_MOUSEMOVE, WM_LBUTTONDOWN and all other wParam values, we can see that they're consecutive hex values.

			Therefore for our purposes, we can simply package this param as our command. In the same way we package the keyboard vkCode.
			
			The extra data from the struct means we have to decide how to break down the data we'll be sending.
			
			For mouse movement:
				> We want to set the mouse position to a specific location.
				> With every message, we simply check if our mouse position has tried to change by simply checking for the difference.
					>> If there's a difference in location, we send a mouse move message to the client.

			For scrolling and extra buttons, the mouseData property of the struct will contain our info:
				> For scrollwheel, the high-order word is the wheel	DELTA. A positive value indicates that the wheel rotated FORWARD (up, away from the user); a negative value indicates that the wheel rotated BACKWARD (dow, towards the user).
					>> The low-order word is reserved. 
				> For wheel click, the low-order word is WHEEL_DELTA.

				> Any other message has to do with the extra buttons on a mouse. The high-order word specifies which X button was pressed/released. This value can be:
						|-- XBUTTON1 (0x0001) 
						|-- XBUTTON2 (0x0002)
					>> The low-order word is reserved. 

			We don't care about any other info, as the flags are only related to injection.
		*/
		short delta = 0;
		switch (wParam)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
			std::cout << "Left or Right button pressed." << std::endl;
			PostQuitMessage(0);
			break;
		case WM_MOUSEMOVE:
			
			std::cout << "Current mouse position:" << std::endl;
			std::cout << "\t|---- X:\t" << mPos.x << std::endl;
			std::cout << "\t|---- Y:\t" << mPos.y << std::endl;

			std::cout << "Moving by:" << std::endl;
			std::cout << "\t|---- X:\t" << mPos.x - msStruct.pt.x << std::endl;
			std::cout << "\t|---- Y:\t" << mPos.y - msStruct.pt.y << std::endl;
			break;
		case WM_MOUSEWHEEL:
			std::cout << "Mouse wheel event:"<< std::endl;
			delta = HIWORD(msStruct.mouseData);
			std::cout << "\t|---- DELTA:\t" << delta << std::endl;
			std::cout << "\t|---- DIRECTION:\t" << (delta > 0 ? "UP" : "DOWN") << std::endl;
			break;
		default:
			std::cout << "Mouse event not accounted for: " << wParam << std::endl;
			break;
		}

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
mslearn_main()
{

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	std::cout << "Screen size is: " << width << "x" << height << std::endl;
	mPos.x = width / 2;
	mPos.y = height / 2;
	SetCursorPos(mPos.x,mPos.y);
	//GetCursorPos(&mPos);
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
	return 1;
}