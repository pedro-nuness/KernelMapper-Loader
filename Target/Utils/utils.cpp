#include "utils.h"
#include <Windows.h>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")

void Utils::Download( std::string path , std::string url ) {
	URLDownloadToFile( NULL , url.c_str( ) , path.c_str( ) , 0 , NULL );
}


bool Utils::CheckStrings( std::string bString1 , std::string bExpectedResult )
{
	size_t found = bString1.find( bExpectedResult );
	if ( found != std::string::npos )
	{
		return true;
	}

	return false;
}