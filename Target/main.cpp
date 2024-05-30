#include <iostream>
#include "Memory/Memory.h"
#include "Globals/globals.h"
#include "Utils/utils.h"
#include <filesystem>
#include "auth.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace KeyAuth;

api KeyAuthApp( Globals::Get().name , Globals::Get( ).ownerid , Globals::Get( ).secret , Globals::Get( ).version , Globals::Get( ).url );

int main( )
{
	std::cout <<  "\n\n Connecting...\n" ;
	KeyAuthApp.init( );
	if ( !KeyAuthApp.data.success )
	{
		std::cout <<  "\n Status: " << KeyAuthApp.data.message;
		Sleep( 1500 );
		exit( 0 );
	}

	if ( KeyAuthApp.checkblack( ) )
		abort( );

	auto CommunicationFolder = std::string( ( "c:\\Windows\\Communication\\" ) );

	std::string JsonFile;

	std::cout << ( "[+] Reading files on current path!\n" );

	for ( const auto & entry : std::filesystem::recursive_directory_iterator( CommunicationFolder ) )
	{
		if ( Utils::Get().CheckStrings( entry.path( ).string( ) , ( ".json" ) ) )
		{
			JsonFile = entry.path( ).string( );
			break;
		}
	}

	if ( JsonFile.empty( ) )
		exit( 0 );

	std::cout << ( "[+] Found json file!\n" );

	std::ifstream i( JsonFile );
	json j;
	i >> j;
	i.close( );
	Sleep( 500 );

	std::cout << ( "[+] Sucessfully read json file\n" );

	remove( JsonFile.c_str( ) );
	std::cout << ( "[+] Attemping login!\n" );

	KeyAuthApp.login( j[ "user" ] , j[ "password" ] );

	if ( !KeyAuthApp.data.success )
	{
		std::cout << ( "\n Status: " ) << KeyAuthApp.data.message << std::endl;
		Sleep( 1500 );
		exit( 0 );
	}
	std::cout << ( "[+] Logged In!\n" );

	std::cout << ( "[+] Communicating with loader!\n" );

	std::ofstream outfile( CommunicationFolder + ( "ready.json" ) );
	outfile.close( );

	std::cout << ( "[+] Waiting loader response!\n" );

	while ( true )
	{
		if ( !Memory::Get().IsProcessRunning( j[ "loaderpid" ] ) )
		{
			exit( 0 );
		}

		if ( IsDebuggerPresent( ) )
		{
			exit( 0 );
		}

		Sleep( 2000 );
	}

	return 1;
}