#include <iostream>
#include <Windows.h>
/*
	This file contains sample code grabbed from:
	https://www.unknowncheats.me/forum/c-and-c-/83707-setwindowshookex-example.html

	Breaking down this code helped me understand the windows Hook system.
*/

//	Not fully sure on HHOOK
HHOOK				_hook;
//	Windows provides helper structs you can cast your data to.
//	https://docs.microsoft.com/en-us/windows/win32/winmsg/hook-structures
KBDLLHOOKSTRUCT		kbdStruct;

BOOL		simulated = false;

LRESULT
CALLBACK	//	CALLBACK is not a keyword. It's simply a preprocessor macro that gets replaced with the appropriate calling convention, which as you already noted is __stdcall.
HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	/*
		https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644985(v=vs.85)
		A code the hook procedure uses to determine how to process the message. If nCode is less than zero, the hook procedure must pass the message to the CallNextHookEx function without further processing and should return the value returned by CallNextHookEx.
	*/
	if (nCode >= 0)
	{
		/*
			wPARAM:
			The identifier of the keyboard message. This parameter can be one of the following messages: WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP.
		*/
		kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
		INPUT inputs[1] = {};
		ZeroMemory(inputs, sizeof(inputs));
		inputs[0].type = INPUT_KEYBOARD;
		//inputs[0].ki.wVk = 0x4C;
		inputs[0].ki.wVk = kbdStruct.vkCode;
		
		
		if (wParam == WM_KEYDOWN)
		{
			/*
				lPARAM:
				Contains the data/struct pertaining to our hook code.
				In this case, KBDLLHOOKSTRUCT.
				We cast our lParam accordingly so we may process the information.
			*/
			std::cout << "KeyDOWN in hook." << std::endl;
			//	vkCode = Virtual Key Code.
			if (kbdStruct.vkCode == VK_ESCAPE)
			{
				
				MessageBox(
					NULL,
					(LPCWSTR)L"Escape was pressed!",
					(LPCWSTR)L"Key pressed",
					MB_ICONINFORMATION);
			}

		}
		else if (wParam == WM_KEYUP)
		{
			std::cout << "KeyUP in hook." << std::endl;
			inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
		}
		
		if (simulated)
		{
			simulated = false;
			UINT uSent = SendInput(1, inputs, sizeof(INPUT));
			if (uSent != ARRAYSIZE(inputs))
			{
				std::cerr << "SendInput Failed :" << HRESULT_FROM_WIN32(GetLastError()) << std::endl;
			}
		}
		else simulated = true;
		
	}
	return CallNextHookEx(_hook, nCode, wParam, lParam);
	/*
		FILTERING INPUT:
		As it turns out, the best way to stop the input from being processed by any other application, is to return without calling CallNextHookEx().
		We simply return 1.
	*/
	//return 1;
}

void
SetHook()
{
	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
		MessageBox(
			NULL,
			(LPCWSTR)L"Failed to install hook!",
			(LPCWSTR)L"Error",
			MB_ICONERROR);
	}
}

void
ReleaseHook()
{
	UnhookWindowsHookEx(_hook);
}

int 
main()
{

	SetHook(); 

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
		DispatchMessageW(&msg);
	}
	UnhookWindowsHookEx(_hook);
}