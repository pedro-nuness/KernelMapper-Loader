#include <iostream>
#include <direct.h>
#include <random>
#include <filesystem>

class TempFiles
{
	bool bSetuped = false;

	std::string Temp;
	std::string RandomFolder;
	std::string CommunicationFolder;

	void CreatePaths( )
	{
		this->CommunicationFolder = CommunicationFolder;
		this->RandomFolder = Temp + RandomFolder + crypt_str( "\\" );

		_mkdir( this->CommunicationFolder.c_str( ) );
		_mkdir( this->RandomFolder.c_str( ) );
	}


public:

	TempFiles( std::string Temp , std::string RandomFolder , std::string CommunicationFolder ) {
		this->bSetuped = true;
		this->Temp = Temp;
		this->RandomFolder = RandomFolder;
		this->CommunicationFolder = CommunicationFolder;
		this->CreatePaths( );
	}

	std::string GetTempFolder( ) {
		if ( bSetuped )
			return Temp;

		return std::string( "" );
	}

	std::string GetRandomFolder( ) {
		if ( bSetuped )
			return RandomFolder;

		return std::string( "" );
	}

	std::string GetCommunicationFolder( ) {
		if ( bSetuped )
			return CommunicationFolder;

		return std::string( "" );
	}

};


