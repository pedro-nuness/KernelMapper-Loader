


#include <iostream>
#include <thread>
#include "DriverCommunication.h"
#include <sstream>
#pragma warning(disable:4996)

DriverCommunication Kernel;
std::vector<AnswareManager> Calls;

bool bPendingLog = false;

namespace Game 
{
	uint64_t Base;

}

void CheckCalls() {

	int OldCallSize = -1;

	while (true) {
		for (int i = 0; i < Calls.size(); i++) {
			if (i > OldCallSize) {
				OldCallSize = i;
				//Calls.at(i).GetAnswer(nAnswer, nSucess);
				std::cout << "DRIVER[" << i << "]: " << Calls.at(i).GetAnswer() << ", sucess: " << Calls.at(i).GetStatus() << std::endl;
				Calls.at(i).finish();
				bPendingLog = false;
			}
		}
		Sleep(150);
	}
}

void SetupCall(AnswareManager* call)
{
	Calls.push_back(*call);
	call->kResponse = nullptr;
	call->bAnswer = true;
	call->kStatus = false;
	bPendingLog = true;
}

void WaitForLog(std::string log) {

	while (bPendingLog)
		Sleep(250);

	std::cout << log;
}

template <typename T>
void Log(std::string LOG, T info ) {
	std::stringstream Adresses;
	std::string LOGS;

	if (info) {
		Adresses << info;
		LOGS = LOG + ": " + Adresses.str() + "\n";
	}
	else
		LOGS = LOG;

	WaitForLog(LOGS);
	Adresses.clear();
	LOGS.clear();
}

uint64_t HexAddition(uint64_t h1, uint64_t h2) {
	std::stringstream ss;




}


uint64_t GetBase(HANDLE PID) {
	return Kernel.ReadMemory<uint64_t>(Game::Base , PID);
}

uint64_t GetLocalPlayer() { 
	return Game::Base + 0x18AC00;
}

uint64_t GetHealth(HANDLE PID) {
	return Kernel.ReadMemory<uint64_t>((Game::Base + 0x18AC00) + 0xEC, PID);
}


int main() {

	auto KernelCheck = Kernel.CheckDriver();
	if (!KernelCheck.kStatus) {
		std::cout << "Can't get driver response!\n";
		Sleep(2000);
		exit(0);
	}

	std::thread(CheckCalls).detach();

	Calls.push_back(KernelCheck);

	Sleep(2000);

	std::string process;

	Log("Target process: ", NULL);
	std::cin >> process;
	Log("\n", NULL);

	if (process.empty())
	{
		Log("Invalid Process!\n", NULL);
		Sleep(2000);
		exit(0);
	}

	AnswareManager GlobalCall = AnswareManager(true);
	auto PID = Kernel.GetProcessPID(process.c_str(), &GlobalCall);
	SetupCall(&GlobalCall);

	Log("PID", PID);

	std::string wantmodule;
	Log("Module: ", NULL);
	std::cin >> wantmodule;
	Log("\n", NULL);

	void* Module = Kernel.GetModuleBaseAdress(wantmodule.c_str(), PID, &GlobalCall);
	SetupCall(&GlobalCall);

	Log("Module", Module);

	Sleep(100);

	Game::Base = reinterpret_cast<uint64_t>(Module);
	if (!Game::Base)
	{
		Sleep(2000);
		exit(0);
	}

	//Kernel.ReadMemory<uint64_t>(Game::Base, PID);
	std::cout << "Game base memory: ";
	std::cout << "0x" << std::hex << Game::Base;
	std::cout << "\n";
	
	//SetupCall(&GlobalCall);
	bPendingLog = false;


	auto Base = GetBase(PID);


	//uint64_t local = Game::Base + 0x18AC00;
	std::cout << "Local: " << std::hex << Kernel.ReadMemory<uint64_t>(Game::Base, PID);
	//Log("Base", Base);
	//std::cout << Health;




	//auto PEB = Kernel.GetProcessPeb(PID, &GlobalCall); // Driver 1
	//SetupCall(&GlobalCall);

	//std::cout << "PEB ADDR: " << std::hex << PEB << std::dec << std::endl;
	//system("pause");

	//bool Alive = false;

	//auto OldLife = Kernel.ReadMemory<uint64_t>(0x95DAB88, PID);

	while (true)
	{
	//	uint64_t ammon = 30;
	//	uint64_t exhealth = 999;

	//	auto CurrentLife = Kernel.ReadMemory<uint64_t>(0x074132C, PID);

	//	if (CurrentLife - OldLife) {
	//		Kernel.WriteMemory<uint64_t>(0x95DAB88, exhealth, PID);
	//	}

	//	auto ammo = Kernel.WriteMemory<uint64_t>(0x0741378, ammon, PID);

	//	Sleep(150);
	}

	//std::cout << "PID: " << PID << "\n";

	//auto ModuleBase = Kernel.GetModuleBase(process.c_str(), PID);
	//if(!ModuleBase)
	//{
	//	std::cout << "Failed to get ModuleBase!\n";
	//	Sleep(2000);
	//	exit(0);
	//}

	//std::cout << "ModBase: " << std::hex << ModuleBase << std::dec << std::endl;
	//system("pause");

	return 1;
}
