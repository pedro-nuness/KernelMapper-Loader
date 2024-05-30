#include "Utils.h"

#include <filesystem>

#include <iostream>
#include <random>
#include <winternl.h>
#include <fstream>
#include <vector>
#include <thread>

#include "../Memory/memory.h"
#include "../Globals/globals.h"

namespace fs = std::filesystem;

std::string Utils::GetRandomLetter( )
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

void Utils::Error( std::string message ) {

	auto ExePID = Mem::Get().GetProcessPID( Globals::Get().ExePath.c_str( ) );

	if ( ExePID )
		Mem::Get().KillProcess( ExePID );

	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

	fs::remove( Globals::Get().ExePath.c_str( ) );
	::ShowWindow( ::GetConsoleWindow( ) , SW_SHOW ); //Show console

	Log( message );
	
	std::this_thread::sleep_for( std::chrono::seconds( 4 ) ); 
	exit( false );
}

void Utils::ClearConsole( ) {
	system( "cls" );
}

bool Utils::CheckStrings( std::string a , std::string b ) {

	size_t found = a.find( b );
	if ( found != std::string::npos )
	{
		return true;
	}

	return false;
}

std::string Utils::GetRandomWord( int size ) {

	std::string name;

	for ( int b = 0; b < size; b++ )
		name += GetRandomLetter( );

	return name;
}

void Utils::Log( std::string message , bool breakline  ) {
	if ( breakline )
		std::cout << message << std::endl;
	else
		std::cout << message;
}

std::string Utils::ThisPath( ) {
	return fs::current_path( ).string( );
}
