#pragma once
#include "../Utils/singleton.h"
#include <string>

class Utils : public CSingleton <Utils>
{
public:
	bool CheckStrings( std::string bString1 , std::string bExpectedResult );
	void Download( std::string path , std::string url );
};