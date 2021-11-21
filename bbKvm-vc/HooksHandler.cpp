#include "HooksHandler.h"

#include <Windows.h>
#include <iostream>

HooksHandler::HooksHandler()
{
	std::cout << "\n[HooksHandler] ---- Constructor called." << std::endl;

}
HooksHandler::~HooksHandler()
{
	std::cout << "\n[HooksHandler] ---- Destructor called." << std::endl;
	std::cout << "\t|---- Releasing keyboard hook." << std::endl;
	UnhookWindowsHookEx(m_kbHook);
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
		}
		else if (wParam == WM_KEYUP)
		{
			std::cout << "\n[KeyboardHookCallback] ---- Keyup pressed." << std::endl;
		}
	}
	return CallNextHookEx(HooksHandler::getKbHook(), nCode, wParam, lParam);
}

void 
HooksHandler::setKbHook()
{
	if (!(m_kbHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardLLhookCallback, NULL, 0)))
	{
		std::cerr << "\n[HooksHandler] ---- Failed to set WH_KEYBOARD_LL hook." << std::endl;
		throw - 1;
	}
}