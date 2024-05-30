#include "Memory.h"

#include <fstream>     // For std::ifstream
#include <cstdint>     // For uint8_t
#include <tlhelp32.h>  // For Process32First, Process32Next, PROCESSENTRY32, CreateToolhelp32Snapshot

bool Memory::ReadFileToMemory( const std::string & file_path , std::vector<uint8_t> * out_buffer )
{
	std::ifstream file_ifstream( file_path , std::ios::binary );

	if ( !file_ifstream )
		return false;

	out_buffer->assign( ( std::istreambuf_iterator<char>( file_ifstream ) ) , std::istreambuf_iterator<char>( ) );
	file_ifstream.close( );

	return true;
}


DWORD Memory::GetProcessPID( LPCTSTR ProcessName ) // non-conflicting function name
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS , 0 );
	pt.dwSize = sizeof( PROCESSENTRY32 );
	if ( Process32First( hsnap , &pt ) ) { // must call this first
		do {
			if ( !lstrcmpi( pt.szExeFile , ProcessName ) ) {
				CloseHandle( hsnap );
				return pt.th32ProcessID;
			}
		} while ( Process32Next( hsnap , &pt ) );
	}
	CloseHandle( hsnap ); // close handle on failure
	return 0;
}

BOOL Memory::IsProcessRunning( DWORD pid )
{
	HANDLE process = OpenProcess( SYNCHRONIZE , FALSE , pid );
	DWORD ret = WaitForSingleObject( process , 0 );
	CloseHandle( process );
	return ret == WAIT_TIMEOUT;
}
