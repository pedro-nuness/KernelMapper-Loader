#include "memory.h"

#include <fstream>
#include <tlhelp32.h>
#include <psapi.h>
#include <tchar.h>
#include <stdio.h>

#include "../Utils/SHA/sha1.h"
#include "../Utils/Utils.h"
#include "../Utils/crypt_str.h"

bool Mem::KillProcess( int PID ) {
	HANDLE hProcess = OpenProcess( PROCESS_TERMINATE , FALSE , PID );

	if ( hProcess == NULL ) {
		return false;
	}
	else
	{
		// Fechar o processo
		TerminateProcess( hProcess , 0 );

		// Liberar o handle do processo
		CloseHandle( hProcess );
	}
	return true;
}

bool Mem::ReadFileToMemory( const std::string & file_path , std::vector<uint8_t> * out_buffer )
{
	std::ifstream file_ifstream( file_path , std::ios::binary );

	if ( !file_ifstream )
		return false;

	//out_buffer->assign((std::istreambuf_iterator<char>(file_ifstream)), std::istreambuf_iterator<char>());
	file_ifstream.close( );

	return true;
}


std::vector<std::string> Mem::GetDrivers( ) {
	const int ARRAY_SIZE = 1024;

	LPVOID drivers[ ARRAY_SIZE ];
	DWORD cbNeeded;
	int cDrivers , i;

	std::vector<std::string> Drivers;

	if ( EnumDeviceDrivers( drivers , sizeof( drivers ) , &cbNeeded ) && cbNeeded < sizeof( drivers ) )
	{
		TCHAR szDriver[ ARRAY_SIZE ];

		cDrivers = cbNeeded / sizeof( drivers[ 0 ] );

		for ( i = 0; i < cDrivers; i++ )
		{
			if ( GetDeviceDriverFileNameA( drivers[ i ] , szDriver , sizeof( szDriver ) / sizeof( szDriver[ 0 ] ) ) )
			{
				Drivers.emplace_back( szDriver );
			}
		}
	}
	else
	{
		Utils::Get( ).Error( crypt_str( "can't get drivers, small array" ) );
	}

	return Drivers;
}


std::string Mem::GetFileHash( std::string & path ) {
	std::vector<uint8_t> CurrentBytes;
	if ( !ReadFileToMemory( path , &CurrentBytes ) ) {
		Utils::Get( ).Error( crypt_str( "can't read file memory!" ) );
	}

	SHA1 sha1;
	sha1.add( CurrentBytes.data( ) + 0 , CurrentBytes.size( ) );
	return sha1.getHash( );
}

DWORD Mem::GetProcessPID( LPCTSTR ProcessName ) {
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