#include "ScreenInfo.h"
#include <iostream>


ScreenInfo::ScreenInfo()
{
	/*
		The whole program needs to be DPI aware so that the coordinates the system calls serve us match the coordinates passed by the hook.
	*/
	SetProcessDPIAware();

	//	For the time being we only care about our main screen, as that's where we'll lock the mouse.
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_screen.m_width = screenWidth;
	m_screen.m_height = screenHeight;


	m_mPos.x = screenWidth / 2;
	m_mPos.y = screenHeight / 2;
	SetCursorPos(m_mPos.x, m_mPos.y);

}

bool
ScreenInfo::lockMouse()
{
	s_lock = true;
	SetCursorPos(m_mPos.x, m_mPos.y);
	return s_lock;
}

bool
ScreenInfo::unlockMouse()
{
	s_lock = false;
	return s_lock;
}

bool
ScreenInfo::isLocked()
{
	return s_lock;
}

POINT 
ScreenInfo::coords() { 
	return m_mPos; 
}