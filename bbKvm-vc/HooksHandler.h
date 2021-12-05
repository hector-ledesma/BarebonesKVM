#pragma once
#include <Windows.h>

class HooksHandler
{
public:
	HooksHandler();
	~HooksHandler();

	//static HHOOK getKbHook() { return m_kbHook; };
	void setHooks();
	void setMsHook();
	//static HHOOK m_kbHook;
private:
	void setKbHook();
};

