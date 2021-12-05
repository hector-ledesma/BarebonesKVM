#include "ScreenInfo.h"
#include <iostream>


ScreenInfo::ScreenInfo()
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	screen.m_width = screenWidth;
	screen.m_height = screenHeight;

	mPos.x = screenWidth / 2;
	mPos.y = screenHeight / 2;

	s_isLocked = false;
}

bool
ScreenInfo::lockMouse()
{
	SetCursorPos(mPos.x, mPos.y);
	s_isLocked = true;
}