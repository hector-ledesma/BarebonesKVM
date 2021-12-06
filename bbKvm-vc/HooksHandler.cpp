#include "Server.h"
#include "ScreenInfo.h"
#include "HooksHandler.h"

#include <iostream>
#include <bitset>

#define		BBKVM_BASE			(WM_USER + 0)

#define		BBKVM_MOUSEMOVE		(BBKVM_BASE + 1)
#define		BBKVM_MOUSEWHEEL	(BBKVM_BASE + 2)
#define		BBKVM_XBUTTON		(BBKVM_BASE + 3)

#define		BBKVM_SIMULATEKEY	(BBKVM_BASE + 4)

HHOOK g_kbHook;
HHOOK g_msHook;

ScreenInfo* g_screen = &ScreenInfo::getInstance();

HooksHandler::HooksHandler()
{
	std::cout << "\n[HooksHandler] ---- Constructor called." << std::endl;

}
HooksHandler::~HooksHandler()
{
	std::cout << "\n[HooksHandler] ---- Destructor called." << std::endl;
	std::cout << "\t|---- Releasing keyboard hook." << std::endl;
	UnhookWindowsHookEx(g_kbHook);
	std::cout << "\t|---- Releasing mouse hook." << std::endl;
	UnhookWindowsHookEx(g_msHook);
}

/*
	It seems that, passing objects or objects initialized within this function down into the message pipeline won't work.

	This makes sense, as the message pipeline is designed to support IPC which rely on different address spaces.

	We need to pick the data from the hook necessary, and create WPARAM and LPARAM variables that hold the data and pass it down the pipeline.

	1.) Virtual Code (vkCode)	->	Determines the key to be pressed.
	2.) Flags					->	Determines the behavior of the key press.

		|---- Extended			->	LLKHF_EXTENDED on KBDLLHOOKSTRUCT and KEYEVENTF_EXTENDEDKEY on INPUT. Indicates if key originated from one of the additional keys on the enhanced keyboard: (right)ALT, (right)CTRL, INS, DEL, HOME, END, PAGE UP, PAGE DOWN, NUM LOCK, arrow keys, BREAK (ctrl + pause), PRINT SCRN, divide(/), ENTER.

		|---- Transition/KeyUp	->	KEYEVENTF_KEYUP on INPUT and LLKHF_UP on KBDLLHOOKSTRUCT. Indicates whether the key is being pressed or released.

		// TODO //
		|---- Alt down			->	Indicates whether the ALT key is pressed.

		// NOTE //
		Normal key press messages contain a Previous Key-State Flag (KF_REPEAT). The hook message does not contain this information. Therefore, we'll never generate repeat messages.
		Fun Fact: The barrier codebase actually contains a note about exactly this, which is just now making sense to me now that I understand this lol.
*/
LRESULT
CALLBACK
keyboardLLhookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode == HC_ACTION)
	{
		KBDLLHOOKSTRUCT *kbdStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
		WPARAM id = wParam;
		WPARAM wParam = kbdStruct->vkCode;		//	We want to keep the virtual key code intact. So store it as our wParam.

		//	For the time being, we'll use the tab key to handle locking and unlocking the main screen which will determine which device we control.
		if (id == WM_KEYUP && wParam == VK_TAB)
		{
			std::cout << "\n[KeyboardHookCallback] ---- Tab key pressed." << std::endl;

			if (g_screen->isLocked())
			{
				std::cout << "\t|---- Unlocking mouse." << std::endl;
				g_screen->unlockMouse();
			}
			else
			{
				std::cout << "\t|---- Locking mouse." << std::endl;
				g_screen->lockMouse();
			}
		}

		if (wParam == VK_ESCAPE)
		{
			std::cout << "\n[KeyboardHookCallback] ---- ESC pressed, terminating program." << std::endl;
			PostQuitMessage(0);
		}

		if (!g_screen->isLocked())
		{
			LPARAM lParam = 0;					//	This value will contain all of our flags. Specific bits will be set to 1.
		//	For all the bitshifting, I'm copying the amount of shifting done from the barrier codebase.
		//	This seems to be arbitrary. It just needs to be consistent across the application.
			std::cout << "\n[KeyboardHookCallback] ---- hook flags = \t\t" << std::bitset<32>(kbdStruct->flags) << std::endl;
			if (kbdStruct->flags & LLKHF_EXTENDED)
			{
				std::cout << "[KeyboardHookCallback] ---- LLKHF_EXTENDED in flag = \t" << std::bitset<32>(1lu << 24) << std::endl;
				lParam |= (1lu << 24);				//	extended key
			}
			if (kbdStruct->flags & LLKHF_ALTDOWN)
			{
				std::cout << "[KeyboardHookCallback] ---- LLKHF_ALTDOWN in flag = \t" << std::bitset<32>(1lu << 29) << std::endl;
				lParam |= (1lu << 29);				//	context code
			}
			if (kbdStruct->flags & LLKHF_UP)
			{
				std::cout << "[KeyboardHookCallback] ---- LLKHF_UP in flag = \t\t" << std::bitset<32>(1lu << 31) << std::endl;
				lParam |= (1lu << 31);				//	transition
			}

			if (wParam == VK_ESCAPE)
			{
				std::cout << "\n[KeyboardHookCallback] ---- ESC pressed, terminating program." << std::endl;
				PostQuitMessage(0);
			}
			else
			{
				std::cout << "[KeyboardHookCallback] ---- vkCode = " << wParam << std::endl;
				std::cout << "[KeyboardHookCallback] ---- message flags = \t\t" << (ULONG)lParam << std::endl;
				std::cout << "[KeyboardHookCallback] ---- binary = \t\t\t" << std::bitset<32>(lParam) << std::endl;
				PostMessage(nullptr, BBKVM_SIMULATEKEY, wParam, lParam);
			}
			return 1;
		}
	}
	return CallNextHookEx(g_kbHook, nCode, wParam, lParam);
}

/*
	Simulating mouse input is giong to require checking quite a few different flags.

	The biggest complications arise due to how many different things can seemingly happen at once for any given hook trigger. Mouse movement (two variables), combined with potential mouse data for middle mouse/scroll wheel or side buttons trigger. As well as multiple flags to flip.

	So one message should contain:
		|---- [ LONG ]	X mouse movement
		|---- [ LONG ]	Y mouse movement
		|---- [ DWORD ]	mouseData
				|-- Wheel delta if MOUSEEVENTF_WHEEN
				|-- XBUTTON if MOUSEEVENTF_XDOWN or MOUSEEVENTF_XUP
				|-- 0 if neither
		|---- [ DWORD ]	flags

	We can use shorts for our mouse movement as we'll use relative values. But given that a WPARAM is just a pointer to a u_int, we can't make a double where one half is the mouse move data, and the other half is the mouseData. Therefore:
		|---- WPARAM -> Will contain EITHER [ mouse move data ] or [ mouse data ].
		|---- LPARAM -> Will always contain FLAGS.
*/
LRESULT
CALLBACK
mouseLLhookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION && !g_screen->isLocked())
	{
		MSLLHOOKSTRUCT msStruct = *((MSLLHOOKSTRUCT*)lParam);
		POINT mLock = g_screen->coords();
		WPARAM id = wParam;
		WPARAM wParam = 0;
		LPARAM lParam = 0;

		//	TODO: - Handle flags first, as they'll always be sent even if we're sending both mouse movement and other mouse input.
		switch (id)
		{
		case WM_MOUSEMOVE:
			lParam |= MOUSEEVENTF_MOVE;
			break;
		case WM_LBUTTONDOWN:
			lParam |= MOUSEEVENTF_LEFTDOWN;
			break;
		case WM_LBUTTONUP:
			lParam |= MOUSEEVENTF_LEFTUP;
			break;
		case WM_RBUTTONDOWN:
			lParam |= MOUSEEVENTF_RIGHTDOWN;
			break;
		case WM_RBUTTONUP:
			lParam |= MOUSEEVENTF_RIGHTUP;
			break;
		case WM_MBUTTONDOWN:
			lParam |= MOUSEEVENTF_MIDDLEDOWN;
			break;
		case WM_MBUTTONUP:
			lParam |= MOUSEEVENTF_MIDDLEUP;
			break;
		case WM_XBUTTONDOWN:
			lParam |= MOUSEEVENTF_XDOWN;
			break;
		case WM_XBUTTONUP:
			lParam |= MOUSEEVENTF_XUP;
			break;
		case WM_MOUSEWHEEL:
			lParam |= MOUSEEVENTF_WHEEL;
			break;
		case WM_XBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
		default:
			std::cout << "Mouse event not accounted for: " << wParam << std::endl;
			break;
		}

		//	If there's any mouse movement, send a mouse move message.
		int moveX = mLock.x - msStruct.pt.x;
		int moveY = mLock.y - msStruct.pt.y;
		if (moveX != 0 || moveY != 0)
		{
			wParam = MAKELONG(moveX, moveY);
			PostMessage(nullptr, BBKVM_MOUSEMOVE, wParam, lParam);
		}

		//	Regardless of whether the msg is for mouse wheel or xbutton, we'll extract the HIWORD into our wParam.
		//	What matters to us, is communicating to the client what type of message it was.
		wParam = HIWORD(msStruct.mouseData);
		if (id == WM_MOUSEWHEEL)
		{
			PostMessage(nullptr, BBKVM_MOUSEWHEEL, wParam, lParam);
		}
		else if (id == WM_XBUTTONUP || id == WM_XBUTTONDOWN)
		{
			PostMessage(nullptr, BBKVM_XBUTTON, wParam, lParam);
		}
		// unless it's neither, in which case leave it at 0
		else
		{
			PostMessage(nullptr, BBKVM_XBUTTON, 0, lParam);
		}

		return 1;
	}

	return CallNextHookEx(g_msHook, nCode, wParam, lParam);
}

void
HooksHandler::setHooks()
{
	std::cout << "\n[HooksHandler] ---- Setting up hooks." << std::endl;
	setKbHook();
	setMsHook();
}

void 
HooksHandler::setKbHook()
{

	if (!(g_kbHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardLLhookCallback, NULL, 0)))
	{
		std::cerr << "\n[HooksHandler] ---- Failed to set WH_KEYBOARD_LL hook." << std::endl;
		throw - 1;
	}
	std::cerr << "\t|----  WH_KEYBOARD_LL hook set." << std::endl;
}

void
HooksHandler::setMsHook()
{
	if (!(g_msHook = SetWindowsHookEx(WH_MOUSE_LL, mouseLLhookCallback, NULL, 0)))
	{
		std::cerr << "\n[HooksHandler] ---- Failed to set WH_MOUSE_LL hook." << std::endl;
		throw - 1;
	}
	std::cerr << "\t|----  WH_MOUSE_LL hook set." << std::endl;
}