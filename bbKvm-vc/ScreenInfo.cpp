#include "ScreenInfo.h"
#include <iostream>


ScreenInfo::ScreenInfo()
{
	int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	m_screen.m_width = screenWidth;
	m_screen.m_height = screenHeight;

	m_mPos.x = screenWidth / 2;
	m_mPos.y = screenHeight / 2;

	SetCursorPos(m_mPos.x, m_mPos.y);

	/*POINT truePos;
	GetCursorPos(&truePos);

	m_mPos.x = truePos.x;
	m_mPos.y = truePos.y;
	SetCursorPos(m_mPos.x, m_mPos.y);*/
	s_lock = true;

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
	SetCursorPos(m_mPos.x, m_mPos.y);
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