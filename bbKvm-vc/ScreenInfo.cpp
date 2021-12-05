#include "ScreenInfo.h"
#include <iostream>


ScreenInfo::ScreenInfo()
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_screen.m_width = screenWidth;
	m_screen.m_height = screenHeight;

	m_mPos.x = screenWidth / 2;
	m_mPos.y = screenHeight / 2;

	s_lock = false;
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