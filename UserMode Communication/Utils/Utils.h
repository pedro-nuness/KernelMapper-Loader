#pragma once

#include <Windows.h>
#include "Singleton.h"
#include <string>

class Utils : public CSingleton<Utils>
{
public:
    BOOL IsProcessRunning(DWORD pid);
	DWORD GetProcessPID(LPCTSTR ProcessName);
};
