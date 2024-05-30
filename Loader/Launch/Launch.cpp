#include "Launch.h"

#include <Auth/auth.hpp>

#include <signal.h> 

#include "../Globals/globals.h"
#include "../Utils/crypt_str.h"
#include "../TempFiles/Temp.h"
#include "../Driver/driver.h"
#include "../Memory/memory.h"
//#include "../kdmapper/DriverMapping.h"
#include "nlohmann/json.hpp"
#include "../Memory/Injection/Injection.h"
#include "../Utils/Utils.h"

using json = nlohmann::json;

KeyAuth::api * ApiPtr;

namespace fs = std::filesystem;

extern bool bDestroy;
bool bVerified = false;

std::vector<uint8_t> Launch::GetFileBytes( std::string file_id ) {

	std::vector<std::uint8_t> bytes = ApiPtr->download( file_id );

	if ( !ApiPtr->data.success )
	{
		MessageBox( NULL , ApiPtr->data.message.c_str( ) , std::string( crypt_str( "Warning!" ) ).c_str( ) , MB_OK );
		bytes.clear( );
	}
	return bytes;
}


void Launch::CreateFileBytes( std::string path , std::string FileID )
{
	std::vector<std::uint8_t> bytes = ApiPtr->download( FileID );

	if ( !ApiPtr->data.success ) // check whether file downloaded correctly
	{
		MessageBox( NULL , ApiPtr->data.message.c_str( ) , std::string( crypt_str( "Warning!" ) ).c_str( ) , MB_OK );
		exit( 0 );
	}

	std::ofstream file( path , std::ios_base::out | std::ios_base::binary );
	file.write( ( char * ) bytes.data( ) , bytes.size( ) );
	file.close( );
	bytes.clear( );
}

TempFiles SetupDirectory( )
{
	auto TempPath = std::filesystem::temp_directory_path( ).string( );
	auto RandomFolder = Utils::Get( ).GetRandomWord( 17 );

	return TempFiles( TempPath , RandomFolder , crypt_str( "c:\\Windows\\Communication\\" ) );
}

TempFiles Directories = SetupDirectory( );

void Launch::ClearCommunicationFolder( ) {
	for ( const auto & entry : fs::directory_iterator( Directories.GetCommunicationFolder( ) ) )
	{
		if ( entry.path( ).string( ).empty( ) )
			continue;

		fs::remove( entry.path( ) );

		Sleep( 50 );
	}
}

void Launch::ClearApps( ) {

	for ( const auto & entry : fs::directory_iterator( Utils::Get( ).ThisPath( ) ) )
	{
		std::string filename = entry.path( ).filename( ).string( );

		if ( filename.empty( ) )
			continue;

		if ( Utils::Get( ).CheckStrings( filename , crypt_str( ".exe" ) ) &&
			Utils::Get( ).CheckStrings( filename , crypt_str( "Alyx" ) )
			&& filename.size( ) > 27 )
		{
			fs::remove( entry.path( ) );
		}
	}
}

void Launch::SetupIDS( ) {

	Globals::Get( ).AuthenticHash = ApiPtr->var( crypt_str( "AuthenticTargetHash" ) );
	Globals::Get( ).DriverID = ApiPtr->var( crypt_str( "DriverID" ) );
	Globals::Get( ).BuildID = ApiPtr->var( crypt_str( "BuildID" ) );
	Globals::Get( ).TargetID = ApiPtr->var( crypt_str( "TargetID" ) );
	Globals::Get( ).ResponseID = Utils::Get( ).GetRandomWord( 17 ) + crypt_str( ".json" );

}

void Launch::CreateTarget( ) {
	Globals::Get( ).ExePath = crypt_str( "Alyx" ) + Utils::Get( ).GetRandomWord( 20 ) + crypt_str( ".exe" );
	CreateFileBytes( Globals::Get( ).ExePath , Globals::Get( ).TargetID );
}


void Launch::CheckDriver( ) {
	if ( !Driver::Get( ).CheckDriver( ).GetStatus( ) ) {
		Utils::Get( ).Log( crypt_str( "Driver is not alive" ) );

		int DayZPid = Mem::Get( ).GetProcessPID( crypt_str( "DayZ_x64.exe" ) );
		if ( DayZPid )
			Mem::Get( ).KillProcess( DayZPid );

		int BEPid = Mem::Get( ).GetProcessPID( crypt_str( "DayZ_BE.exe" ) );
		if ( BEPid )
			Mem::Get( ).KillProcess( BEPid );

		int LauncherPID = Mem::Get( ).GetProcessPID( crypt_str( "Launcher.exe" ) );
		if ( LauncherPID )
			Mem::Get( ).KillProcess( LauncherPID );

		std::vector<uint8_t> DriverMemory = GetFileBytes( Globals::Get( ).DriverID );
		if ( DriverMemory.empty( ) ) {
			Utils::Get( ).Error( crypt_str( "Unable to get driver memory!" ) );
		}

		bool _Response = MapDriver( DriverMemory );
		DriverMemory.clear( );

		Utils::Get( ).ClearConsole( );

		if ( !_Response ) {
			Utils::Get( ).Error( crypt_str( "Unable to map driver!" ) );
		}
		else if ( !Driver::Get( ).CheckDriver( ).GetStatus( ) ) {
			Utils::Get( ).Error( crypt_str( "Driver is not alive!" ) );
		}
	}
}

bool Launch::IsHashAuthentic( std::string hash ) {
	return hash == Globals::Get( ).AuthenticHash;
}

void Launch::CheckHash( ) {
	Globals::Get( ).TargetHash = Mem::Get( ).GetFileHash( Globals::Get( ).ExePath );

	if ( Globals::Get( ).TargetHash.empty( ) ) {
		Utils::Get( ).Error( crypt_str( "Can't allocate hash!" ) );
	}

	if ( !IsHashAuthentic( Globals::Get( ).TargetHash ) ) {
		ApiPtr->ban( crypt_str( "invalid hash" ) );
		std::ofstream file;
		file.open( crypt_str( "loaderoutput.txt" ) );
		file << Globals::Get( ).TargetHash;
		file.close( );
		Utils::Get( ).Error( crypt_str( "Invalid Session!\n" ) + Globals::Get( ).TargetHash );
	}
}

void Launch::CreateJson( ) {
	Globals::Get( ).JsonPath = Directories.GetCommunicationFolder( ) + Utils::Get( ).GetRandomWord( 17 ) + ".json";

	json j;
	j[ crypt_str( "random" ) ] = Directories.GetRandomFolder( );
	j[ crypt_str( "loaderpid" ) ] = GetCurrentProcessId( );
	j[ crypt_str( "user" ) ] = Globals::Get( ).user_name;
	j[ crypt_str( "password" ) ] = Globals::Get( ).pass_word;
	j[ crypt_str( "response" ) ] = Globals::Get( ).ResponseID;

	std::ofstream outfile;
	outfile.open( Globals::Get( ).JsonPath.c_str( ) );
	outfile << j;
	outfile.close( );
}

void Launch::Injection( )
{
	bDestroy = true; //Destroy our gui

	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

	Globals::Get( ).TargetPID = Mem::Get( ).GetProcessPID( Globals::Get( ).ExePath.c_str( ) );

	if ( !Globals::Get( ).TargetPID ) {
		Utils::Get( ).Error( crypt_str( "Can't communicate with target!" ) );
	}

	std::string Response = Directories.GetCommunicationFolder( ) + Globals::Get( ).ResponseID;

	while ( true )
	{
		if ( std::filesystem::exists( Response ) )
		{
			remove( Response.c_str( ) ); //Remove response

			//::ShowWindow( ::GetConsoleWindow( ) , SW_SHOW ); //Show console

			Utils::Get( ).Log( crypt_str( "[+] Waiting for safety verification!" ) );

			while ( !bVerified )
				std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

			Utils::Get( ).Log( crypt_str( "[+] Downloading!" ) );

			if ( Globals::Get( ).Build.empty( ) ) {
				Utils::Get( ).Error( "[ - ] Download Failed!\n" );
			}

			Utils::Get( ).Log( crypt_str( "[+] Finished download!" ) );

			Utils::Get( ).Log( crypt_str( "[+] Mapping!" ) );

			if ( Injector::Get( ).InjectBytes( Globals::Get( ).Build , Globals::Get( ).TargetPID ) )
			{		
				Utils::Get( ).ClearConsole( );
				Globals::Get( ).Build.clear( );
				Utils::Get( ).Log( crypt_str( "Sucessfully mapped!" ) );		
				break;
			}
			else
			{
				Utils::Get( ).ClearConsole( );
				Globals::Get( ).Build.clear( );
				Utils::Get( ).Error( crypt_str( "Mapping failed" ) );
			}
		}

		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	}
}

void Launch::Idle( ) {

	while ( true )
	{
		if ( Globals::Get( ).TargetPID ) {

			auto CurrentPID = Mem::Get( ).GetProcessPID( Globals::Get( ).ExePath.c_str( ) );
			if ( !CurrentPID ) {
				Utils::Get( ).Error( crypt_str( "T: Cheat closed!\n" ) );
			}

			if ( CurrentPID != Globals::Get( ).TargetPID ) {
				ApiPtr->ban( crypt_str( "T: Pid Mismatch detected!" ) );
				Utils::Get( ).Error( crypt_str( "Invalid session!" ) );
			}

			auto CurrentHash = Mem::Get( ).GetFileHash( Globals::Get( ).ExePath );

			if ( !IsHashAuthentic( CurrentHash ) ) {
				ApiPtr->ban( crypt_str( "T: Hash invalidation detected!" ) );
				std::cout << CurrentHash << "\n";
				Sleep( 50000 );
				Utils::Get( ).Error( "Invalid session!" );
			}

			bVerified = true;
		}

		for ( auto process : Globals::Get( ).List )
		{
			if ( Mem::Get( ).GetProcessPID( process.c_str( ) ) ) {
				Utils::Get( ).Error( crypt_str( "T: Non authorized program!" ) );
			}
		}

		if ( IsDebuggerPresent( ) )
		{
			if ( Globals::Get( ).Logged )
				ApiPtr->ban( crypt_str( "Dbg" ) );
			Utils::Get( ).Error( crypt_str( "T: Unsafe Session!" ) );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}

void Launch::Init( void * adress )
{
	ApiPtr = reinterpret_cast< KeyAuth::api * >( adress );

	ClearApps( );
	Utils::Get( ).Log( crypt_str( "path is clear" ) );

	SetupIDS( );
	Utils::Get( ).Log( crypt_str( "id settuped!" ) );

	ClearCommunicationFolder( );
	Utils::Get( ).Log( crypt_str( "folder is clear" ) );

	CheckDriver( );
	Utils::Get( ).Log( crypt_str( "driver is ok" ) );

	CreateTarget( );
	Utils::Get( ).Log( crypt_str( "target created" ) );

	CheckHash( );
	Utils::Get( ).Log( crypt_str( "hash is good" ) );

	Globals::Get( ).Build = GetFileBytes( Globals::Get( ).BuildID );

	CreateJson( );
	Utils::Get( ).Log( crypt_str( "json is good" ) );

	ShellExecute( NULL , "runas" , Globals::Get( ).ExePath.c_str( ) , 0 , 0 , SW_SHOWDEFAULT );
	Utils::Get( ).Log( crypt_str( "target open" ) );

	Sleep( 100 );
	std::thread( &Launch::Injection , this ).detach( );
	Utils::Get( ).Log( crypt_str( "injection" ) );
}
