#include "HooksHandler.h"

#include <iostream>

#define		BB_END				(WM_USER + 0)
#define		SIMULATE_KEYDOWN	(WM_USER + 1)
#define		SIMULATE_KEYUP		(WM_USER + 2)

HHOOK g_kbHook;

HooksHandler::HooksHandler()
{
	std::cout << "\n[HooksHandler] ---- Constructor called." << std::endl;

}
HooksHandler::~HooksHandler()
{
	std::cout << "\n[HooksHandler] ---- Destructor called." << std::endl;
	std::cout << "\t|---- Releasing keyboard hook." << std::endl;
	UnhookWindowsHookEx(g_kbHook);
}

LRESULT
CALLBACK
keyboardLLhookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		KBDLLHOOKSTRUCT *kbdStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

		if (wParam == WM_KEYDOWN)
		{
			std::cout << "\n[KeyboardHookCallback] ---- Keydown pressed." << std::endl;
			if (kbdStruct->vkCode == VK_ESCAPE)
			{
				std::cout << "\n[KeyboardHookCallback] ---- ESC pressed, terminating program." << std::endl;
				PostQuitMessage(0);
			}

			PostMessage(nullptr, SIMULATE_KEYDOWN, wParam, lParam);
		}
		else if (wParam == WM_KEYUP)
		{
			std::cout << "\n[KeyboardHookCallback] ---- Keyup pressed." << std::endl;
			PostMessage(nullptr, SIMULATE_KEYUP, wParam, lParam);

		}
	}
	return CallNextHookEx(g_kbHook, nCode, wParam, lParam);
}

void
HooksHandler::setHooks()
{
	std::cout << "\n[HooksHandler] ---- Setting up hooks." << std::endl;
	setKbHook();
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