#ifndef INJECTOR_H
#define INJECTOR_H

#include "../../Utils/singleton.h"

#include <Windows.h>
#include <vector>

class Injector : public CSingleton<Injector>
{
public:
	bool ManualMapDll( HANDLE hProc , BYTE * pSrcData , SIZE_T FileSize , bool ClearHeader = true , bool ClearNonNeededSections = true , bool AdjustProtections = true , bool SEHExceptionSupport = true , DWORD fdwReason = DLL_PROCESS_ATTACH , LPVOID lpReserved = 0 );
	bool InjectBytes( std::vector<std::uint8_t> bytes , DWORD PID );
};

#endif