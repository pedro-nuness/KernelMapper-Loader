#pragma once
#include "../Utils/singleton.h"
#include <string>


class Globals : public CSingleton <Globals> {
public:
	std::string name = ( "DayzCheat" ); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
	std::string ownerid = ( "OBz74zBgIX" ); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
	std::string secret = ( "" ); // app secret, the blurred text on licenses tab and other tabs
	std::string version = ( "1.0" ); // leave alone unless you've changed version on website
	std::string url = ( "https://keyauth.win/api/1.2/" ); // change if you're self-hosting

	std::string kdu = "428502";
	std::string drv64 = "851567";
	std::string driver = "446426";
	std::string DLL = "610673";
	std::string Starter = "384969";
};



