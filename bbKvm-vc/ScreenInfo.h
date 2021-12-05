#pragma once
#include <Windows.h>
/*
	We'll want to use information about our system's display:
	
	> Main screen's resolution so we may center our mouse 
	> Mouse coordinates for locking mouse in place and calculating how much movement to simulate.
		>> Both of these are also useful if we implement switching devices by dragging mouse out of bounds.

	> We'll also want to contain global functionality related to controlling the device within this class.
		>> e.g.: mouse lock.
*/

class ScreenDimensions
{
public:
	int	m_width = 0;
	int	m_height = 0;
};
class ScreenInfo
{
public:
	ScreenInfo();
	~ScreenInfo();

	/*	
		We can ues this variable across our program to know whether we want to send data over the network or not.
		 >> Locked = communicate input to client. Else, we're controlling this computer.
	*/
	static bool s_isLocked;

	bool lockMouse();

private:
	POINT				mPos;
	ScreenDimensions	screen;
};

