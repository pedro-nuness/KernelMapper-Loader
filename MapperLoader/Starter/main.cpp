#include <iostream>
#include <windows.h>
#include <vector>

#include "sha1.h"

#include <d3d9.h>

bool doOnce = false;

bool show_login = true;
bool show_register = false;
bool show_logged = false;

#include "auth.hpp"
#include <string>
#include "skStr.h"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <Wininet.h>
#include <string>
#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>
#include <filesystem>
#include <random>

#include "json.hpp"
#include <TlHelp32.h>

#include "crypt_str.h";

using json = nlohmann::json;

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")

using namespace KeyAuth;

namespace fs = std::filesystem;


std::string name = ( "DayzCheat" ); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = ( "OBz74zBgIX" ); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = ( "432f0279997bf737f1581324515a5b923d0bcb73d95ca5f47f0399248b037967" ); // app secret, the blurred text on licenses tab and other tabs
std::string version = ( "1.0" ); // leave alone unless you've changed version on website
std::string url = ( "https://keyauth.win/api/1.2/" ); // change if you're self-hosting

api KeyAuthApp( name , ownerid , secret , version , url );

bool CheckStrings( std::string bString1 , std::string bExpectedResult )
{
	size_t found = bString1.find( bExpectedResult );
	if ( found != std::string::npos )
	{
		return true;
	}

	return false;
}

void CreateF( std::string path , std::string FileID )
{
	std::vector<std::uint8_t> bytes = KeyAuthApp.download( FileID );

	if ( !KeyAuthApp.data.success ) // check whether file downloaded correctly
	{
		MessageBox( NULL , KeyAuthApp.data.message.c_str( ) , std::string( ( "Warning!" ) ).c_str( ) , MB_OK );
		exit( 0 );
	}

	std::ofstream file( path , std::ios_base::out | std::ios_base::binary );
	file.write( ( char * ) bytes.data( ) , bytes.size( ) );
	file.close( );
}


bool Valid = false;

BOOL IsProcessRunning( DWORD pid )
{
	HANDLE process = OpenProcess( SYNCHRONIZE , FALSE , pid );
	DWORD ret = WaitForSingleObject( process , 0 );
	CloseHandle( process );
	return ret == WAIT_TIMEOUT;
}

bool ReadFileToMemory( const std::string & file_path , std::vector<uint8_t> * out_buffer )
{
	std::ifstream file_ifstream( file_path , std::ios::binary );

	if ( !file_ifstream )
		return false;

	out_buffer->assign( ( std::istreambuf_iterator<char>( file_ifstream ) ) , std::istreambuf_iterator<char>( ) );
	file_ifstream.close( );

	return true;
}

std::string GetFileHash( std::string path )
{
	std::vector<uint8_t> CurrentBytes;
	if ( !ReadFileToMemory( path , &CurrentBytes ) )
	{
		std::cout << crypt_str( "[+] Can't read target memory!\n" );
		Sleep( 1000 );
		exit( 0 );
	}

	SHA1 sha1;
	sha1.add( CurrentBytes.data( ) + 0 , CurrentBytes.size( ) );
	return sha1.getHash( );
}


bool IsHashAuthentic( api * App , std::string hash )
{
	std::string AuthenticHash = App->var( ( "AuthenticLoaderHash" ) );
	return hash == AuthenticHash;
}


std::string ProcessIdToName( DWORD processId )
{
	std::string ret;
	HANDLE handle = OpenProcess(
		PROCESS_QUERY_LIMITED_INFORMATION ,
		FALSE ,
		processId /* This is the PID, you can find one from windows task manager */
	);
	if ( handle )
	{
		DWORD buffSize = 1024;
		CHAR buffer[ 1024 ];
		if ( QueryFullProcessImageNameA( handle , 0 , buffer , &buffSize ) )
		{
			ret = buffer;
		}
		else
		{
			printf( "Error GetModuleBaseNameA : %lu" , GetLastError( ) );
		}
		CloseHandle( handle );
	}
	else
	{
		printf( "Error OpenProcess : %lu" , GetLastError( ) );
	}
	return ret;
}

std::string RandomLetter( )
{
	std::string letters[ ] = { "a", "b", "c", "d", "e", "f", "g", "h", "i",
					"j", "k", "l", "m", "n", "o", "p", "q", "r",
					"s", "t", "u", "v", "w", "x", "y", "z", "A", "B", "C"
					"D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O"
					"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

	std::random_device r;
	std::seed_seq seed { r( ), r( ), r( ), r( ), r( ), r( ), r( ), r( ) };
	std::shuffle( std::begin( letters ) , std::end( letters ) ,
		std::mt19937( seed ) );

	for ( auto c : letters )
		return c;

}

std::string GetRandomWord( int size )
{
	std::string name;

	for ( int b = 0; b < size; b++ )
		name += RandomLetter( );

	return name;
}



namespace Global {
	std::string CommunicationFolder = std::string( crypt_str( "c:\\Windows\\Communication\\" ) );
	std::string user , pass , loadername , loaderhash , response;
	int loaderpid;

}


bool SetupHash( std::string  File , std::string * Hash ) {

	std::string FileHash = GetFileHash( Global::loadername );

	if ( FileHash.empty( ) ) {
		return false;
	}
	*Hash = FileHash;
	return true;
}

json GetJson( ) {

	json Json;

	std::string JsonFile;
	std::cout << crypt_str( "[+] Reading files on current path!\n" );

	for ( const auto & entry : std::filesystem::recursive_directory_iterator( Global::CommunicationFolder ) )
	{
		if ( CheckStrings( entry.path( ).string( ) , ( ".json" ) ) )
		{
			JsonFile = entry.path( ).string( );
			break;
		}
	}

	if ( JsonFile.empty( ) )
		exit( 0 );

	std::cout << crypt_str( "[+] Found json file!\n" );

	std::ifstream i( JsonFile );

	i >> Json;
	i.close( );
	Sleep( 500 );
	std::cout << crypt_str( "[+] Sucessfully read json file\n" );
	remove( JsonFile.c_str( ) );

	return Json;
}

bool KillProcess( int PID ) {
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

void Log( std::string txt ) {
	std::cout << txt << std::endl;
}

int main( )
{
	std::cout << skCrypt( " Connecting..." );
	KeyAuthApp.init( );
	if ( !KeyAuthApp.data.success )
	{
		std::cout << skCrypt( " Status: " ) << KeyAuthApp.data.message;
		Sleep( 1500 );
		exit( 0 );
	}

	if ( KeyAuthApp.checkblack( ) )
		abort( );

	json Js = GetJson( );
	Global::user = Js[ "user" ];
	Global::pass = Js[ "password" ];
	Global::loadername = ProcessIdToName( Js[ "loaderpid" ] );
	Global::loaderpid = Js[ "loaderpid" ];
	Global::response = Js[ "response" ];

	Log( crypt_str( "[+] Sucessfully Settuped Infos!" ) );

	KeyAuthApp.login( Global::user , Global::pass );

	if ( !KeyAuthApp.data.success )
	{
		std::cout << skCrypt( " Status: " ) << KeyAuthApp.data.message << std::endl;
		Sleep( 1500 );
		exit( 0 );
	}
	Log( crypt_str( "[+] Logged In!" ) );

	if ( !SetupHash( Global::loadername , &Global::loaderhash ) ) {
		Log( crypt_str( "Can't get loader memory!" ) );
		Sleep( 3000 );
		exit( 0 );
	}


	if ( !IsHashAuthentic( &KeyAuthApp , Global::loaderhash ) ) {
		Log( crypt_str( "[!] It seems like your loader isn't authentic!" ) );
		std::ofstream file;
		file.open( "targetoutput.txt" );
		file << Global::loaderhash;
		file.close( );

		if ( KillProcess( Global::loaderpid ) )
		{
			Sleep( 500 );
			remove( Global::loadername.c_str( ) );

			std::string NewLoaderName = crypt_str( "ldru" ) + GetRandomWord( 20 ) + crypt_str( ".exe" );

			for ( const auto & entry : fs::directory_iterator( fs::current_path( ) ) )
			{
				std::string filename = entry.path( ).filename( ).string( );

				if ( filename.empty( ) )
					continue;

				if ( CheckStrings( filename , crypt_str( ".exe" ) ) &&
					CheckStrings( filename , crypt_str( "ldru" ) )
					&& filename.size( ) == 28 )
				{
					remove( entry.path( ) );
				}

			}

			CreateF( NewLoaderName , crypt_str( "618172" ) );
			Sleep( 500 );
			ShellExecute( NULL , "runas" , NewLoaderName.c_str( ) , 0 , 0 , SW_SHOWDEFAULT );
		}

		Sleep( 2000 );
		exit( 0 );
	}

	Log( crypt_str( "[+] Communicating with loader!" ) );


	std::ofstream outfile( Global::CommunicationFolder + Global::response );
	outfile.close( );

	system( "cls" );

	Log( crypt_str( "[+] Waiting loader response!" ) );

	while ( true )
	{
		if ( !IsProcessRunning( Global::loaderpid ) )
		{
			exit( 0 );
		}

		if ( IsDebuggerPresent( ) )
		{
			KeyAuthApp.ban( crypt_str( "Debugger detected" ) );
			Log( crypt_str( "Unsafe session!" ) );
			exit( 0 );
		}

		Sleep( 2000 );
	}

	return 1;
}