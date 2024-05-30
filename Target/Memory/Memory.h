#pragma once
#include "../Utils/singleton.h"
#include <Windows.h>
#include <string>
#include <vector>


class Memory : public CSingleton <Memory>
{
public:
	bool ReadFileToMemory( const std::string & file_path , std::vector<uint8_t> * out_buffer );
	BOOL IsProcessRunning( DWORD pid );
	DWORD GetProcessPID( LPCTSTR ProcessName );
};

