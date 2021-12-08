#include <Windows.h>
#include <iostream>


void
test_main() {

	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dx = 10;
	input.mi.dy = -20;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	SendInput(1, &input, sizeof(INPUT));

	for (int i = 0; i < 20; i++)
	{
		std::cout << "Sending input" << std::endl;
		SendInput(1, &input, sizeof(INPUT));
	}
}