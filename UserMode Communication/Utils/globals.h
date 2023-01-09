#pragma once

class Globals
{
public:
	ULONG64 Client;
	ULONG64 Engine;
	HANDLE PID;

	bool Debug = false;

	DriverCommunication* K = nullptr;
};
extern Globals* Global;