#pragma once
#include <Windows.h>

class ScreenDimensions
{
public:
	int	m_width = 0;
	int	m_height = 0;
};

/*
	We'll want to use information about our system's display:

	> Main screen's resolution so we may center our mouse
	> Mouse coordinates for locking mouse in place and calculating how much movement to simulate.
		>> Both of these are also useful if we implement switching devices by dragging mouse out of bounds.

	> We'll also want to contain global functionality related to controlling the device within this class.
		>> e.g.: mouse lock.

	Ultimately, I decided the best course of action was to have a single global instance for this object. Our hook callback functions sit outside of the scope of any object, and our server will only ever need a single controller class for its screens.
*/
class ScreenInfo
{
private:
	ScreenInfo();

	POINT				m_mPos;
	ScreenDimensions	m_screen;

	/*
		We can ues this variable across our program to know whether we want to send data over the network or not.
		 >> Locked = communicate input to client. Else, we're controlling this computer.
	*/
	bool s_lock;

public:
	static ScreenInfo& getInstance()
	{
		static ScreenInfo instance;
		return instance;
	}
	ScreenInfo(ScreenInfo const&)		= delete;
	void operator=(ScreenInfo const&)	= delete;

	bool lockMouse();
	bool unlockMouse();
	bool isLocked();

	POINT coords();


};

