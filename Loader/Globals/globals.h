#ifndef GLOBALS_H
#define GLOBALS_H


#include <string>
#include <vector>

#include "../Utils/crypt_str.h"
#include "../Utils/singleton.h"

bool MapDriver( std::vector<uint8_t> DriverMemory , bool MdlMode = false , bool Free = false , bool PassAlocationPtr = false );

class Globals : public CSingleton<Globals>
{
public:
	std::string name = crypt_str( "Dayz" ); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
	std::string ownerid = crypt_str( "tqbS9rmJpM" ); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
	std::string secret = crypt_str( "d00254a472ed49b4dd987edd9c779f58215c5a0f7243ae148a85e24bfe91982f" ); // app secret, the blurred text on licenses tab and other tabs
	std::string version = crypt_str( "1.0" ); // leave alone unless you've changed version on website
	std::string url = crypt_str( "https://keyauth.win/api/1.2/" ); // change if you're self-hosting

	std::string TargetID = "";
	std::string BuildID = "";
	std::string DriverID = "";

	std::vector<uint8_t> Build;

	std::string ExePath = "";
	std::string DriverPath = "";
	std::string JsonPath = "";
	std::string TargetHash = "";

	std::string AuthenticHash;

	std::string ResponseID = "";

	std::string Last_Update;

	int TargetPID;
	int Status;

	bool RunningLaunchThread = false;

	char user_name[ 255 ] = "";
	char pass_word[ 255 ] = "";
	char invite_key[ 255 ] = "";

	int button_opacity = 255;

	bool Active = true;
	bool Logged = false;

	std::vector<std::string>
		List {
			crypt_str( "ida64.exe" ),
			crypt_str( "ida32.exe" ),
			crypt_str( "Wireshark.exe" ),
			crypt_str( "ProcessHacker.exe" ),
			crypt_str( "HTTPDebuggerUI" ),
			crypt_str( "Charles.exe" ),
			crypt_str( "x64dbg.exe" ),
			crypt_str( "x32dbg.exe" ),
			crypt_str( "DnSpy.exe" ),
	};


};

#endif // ! GLOBALS_H
