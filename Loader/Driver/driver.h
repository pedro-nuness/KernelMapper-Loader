#ifndef DRIVER_H
#define DRIVER_H


#include <Windows.h>
#include <vector>
#include <string>
#include <mutex>

#include "communication.h"
#include "..\Utils\singleton.h"

class Driver : public CSingleton<Driver>
{
	HANDLE dPid = NULL;
	bool uSetupped = false;

	std::once_flag flag;

public:

	template<typename ... A>
	uint64_t CallHook( const A ... arguments );

	AnswareManager CheckDriver( );
};

#endif // !DRIVER_H