#include "HooksHandler.h"

#include <iostream>
#include <bitset>

#define		BBKVM_SIMULATEKEY	(WM_USER + 0)

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

	if (nCode >= 0)
	{
		KBDLLHOOKSTRUCT *kbdStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
		WPARAM id = wParam;
		WPARAM wParam = kbdStruct->vkCode;		//	We want to keep the virtual key code intact. So store it as our wParam.
		LPARAM lParam = 0;					//	This value will contain all of our flags. Specific bits will be set to 1.
		//	For all the bitshifting, I'm copying the amount of shifting done from the barrier codebase.
		//	This seems to be arbitrary. It just needs to be consistent across the application.
		std::cout << "\n[KeyboardHookCallback] ---- hook flags = \t\t" << std::bitset<32>(kbdStruct->flags) << std::endl;
		if (kbdStruct->flags & LLKHF_EXTENDED)
		{
			std::cout << "[KeyboardHookCallback] ---- LLKHF_EXTENDED in flag = \t" << std::bitset<32>(1lu<<24) << std::endl;
			lParam |= (1lu << 24);				//	extended key
		}
		if (kbdStruct->flags & LLKHF_ALTDOWN)
		{
			std::cout << "[KeyboardHookCallback] ---- LLKHF_ALTDOWN in flag = \t" << std::bitset<32>(1lu<<29) << std::endl;
			lParam |= (1lu << 29);				//	context code
		}
		if (kbdStruct->flags & LLKHF_UP)
		{
			std::cout << "[KeyboardHookCallback] ---- LLKHF_UP in flag = \t\t" << std::bitset<32>(1lu<<31) << std::endl;
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


		/*if (id == WM_KEYDOWN)
		{
			std::cout << "\n[KeyboardHookCallback] ---- Keydown pressed." << std::endl;	
			if (kbdStruct->vkCode == VK_ESCAPE)
			{
				std::cout << "\n[KeyboardHookCallback] ---- ESC pressed, terminating program." << std::endl;
				PostQuitMessage(0);
			}
			else if (kbdStruct->vkCode == 0x41)
				std::cout << "\n[KeyboardHookCallback] ---- Caught the letter 'A'." << std::endl;

			PostMessage(nullptr, SIMULATE_KEYDOWN, wParam, lParam);
		}
		else if (id == WM_KEYUP)
		{
			std::cout << "\n[KeyboardHookCallback] ---- Keyup pressed." << std::endl;
			PostMessage(nullptr, SIMULATE_KEYUP, wParam, lParam);

		}*/
	}
	//return 1;
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