#include <Windows.h>
#include <iostream>

int
test_main()
{
	/*std::cout << GetSystemMetrics(SM_CXSCREEN) << std::endl;
	std::cout << GetSystemMetrics(SM_CYSCREEN) << std::endl;

	std::cout << GetSystemMetrics(SM_CXVIRTUALSCREEN) << std::endl;
	std::cout << GetSystemMetrics(SM_CYVIRTUALSCREEN) << std::endl;*/


	int ax = GetSystemMetrics(SM_CXFULLSCREEN) / 2;
	int ay = GetSystemMetrics(SM_CYFULLSCREEN) / 2;

	std::cout << ax  << std::endl;
	std::cout << ay << std::endl;

	SetCursorPos(ax,ay);
	POINT truePos;
	GetCursorPos(&truePos);
	std::cout << truePos.x << std::endl;
	std::cout << truePos.y << std::endl;

	int x = truePos.x;
	int y = truePos.y;
	SetCursorPos(ax,ay);
	std::cout << x << std::endl;
	std::cout << y << std::endl;

	return 0;
}