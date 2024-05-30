#pragma once
#include <Windows.h>
#include "singleton.h"
#include <string>
#include <vector>

class Utils : public CSingleton<Utils>
{
public:
		
	std::string ThisPath( );
	void Log( std::string message , bool breakline = true );
	void Error( std::string message );
	std::string GetRandomWord( int size );
	std::string GetRandomLetter( );
	bool CheckStrings( std::string a , std::string b );
	void ClearConsole( );
	//bool ReadFileToMemory( const std::wstring & file_path , std::vector<uint8_t> * out_buffer );
	//bool CreateFileFromMemory( const std::wstring & desired_file_path , const char * address , size_t size );
};


