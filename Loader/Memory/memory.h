#ifndef MEMORY_H
#define MEMORY_H

#include <Windows.h>
#include <string>
#include <vector>
#include "../Utils/singleton.h"

class Mem : public CSingleton<Mem>
{
public:
	DWORD GetProcessPID( LPCTSTR ProcessName );
	std::vector<std::string> GetDrivers( );
	std::string GetFileHash( std::string & path );
	bool KillProcess( int PID );
	bool ReadFileToMemory( const std::string & file_path , std::vector<uint8_t> * out_buffer );

};


#endif // !MEMORY_H




