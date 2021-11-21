#pragma once
#include <Windows.h>

class HooksHandler
{
public:
	HooksHandler();
	~HooksHandler();

	static HHOOK getKbHook() { return m_kbHook; };
	void setKbHook();
private:
	static HHOOK m_kbHook;
};

