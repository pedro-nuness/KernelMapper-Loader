

#include "main.h"
#include <vector>

#pragma warning(disable:4996)

DriverCommunication Kernel;

std::vector<AnswareManager> Calls;


bool bPendingLog = false;

Globals* Global;
memory* Memory;


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

template <typename T>
void Log(std::string log, T info) {

	while (bPendingLog)
		Sleep(250);

	if (info)
		std::cout << log << ": " << info << std::endl;
	else
		std::cout << log << std::endl;
}


ULONG64 GetDecFromHex(ULONG64 h, int size) {
	std::stringstream hex_stream;
	hex_stream << std::dec << h;
	std::string hex_sum = hex_stream.str();

	return std::stoi(hex_sum, nullptr, size);
}

std::string hexaddition(std::string hex1, std::string hex2) {
	// Read in the two hexadecimal numbers as strings

	if (hex1.empty() || hex2.empty())
		return "";

	//// Convert the hexadecimal strings to integers
	int num1 = std::stoi(hex1, nullptr, 16);
	int num2 = std::stoi(hex2, nullptr, 16);


	//std::string str = std::to_string(num)
	// 
	// Add the two integers
	int sum = num1 + num2;

	// Convert the sum back to a hexadecimal string
	std::stringstream hex_stream;
	hex_stream << std::hex << sum;
	std::string hex_sum = hex_stream.str();

	std::stringstream output_stream;
	output_stream << std::setw(8) << std::setfill('0') << hex_sum;
	hex_sum = output_stream.str();

	//std::cout <<  hex_sum << std::endl;
	return hex_sum;
}

ULONG64 hexadditionL(ULONG64 long1, ULONG64 long2) {
	// Read in the two hexadecimal numbers as strings

	if (!long1 || !long2)
		return NULL;

	//// Convert the hexadecimal strings to integers
	//int num1 = std::stoi(hex1, nullptr, 16);
	//int num2 = std::stoi(hex2, nullptr, 16);

	//std::string str = std::to_string(num)
	// 
	// Add the two integers
	ULONG64 sum = long1 + long2;

	// Convert the sum back to a hexadecimal string
	std::stringstream hex_stream;
	hex_stream << std::hex << sum;
	std::string hex_sum = hex_stream.str();

	std::stringstream output_stream;
	output_stream << std::setw(8) << std::setfill('0') << hex_sum;
	hex_sum = output_stream.str();

	//std::cout <<  hex_sum << std::endl;
	return std::stoi(hex_sum, nullptr, 16);
}


template<typename t>
t ToHex(t curlong, int size)
{
	std::ostringstream ss;
	ss << std::hex << curlong;
	std::string result = ss.str();

	return std::stoull(result, nullptr, size);
}

void WaitModule(ULONG64& mem, std::string modulename) {

	mem = Kernel.GetModuleBaseAdress(modulename.c_str());

	std::string LOG = "[!] Waiting for " + modulename;

	Log(LOG, NULL);
	while (!mem)
	{
		mem = Kernel.GetModuleBaseAdress(modulename.c_str());
		Sleep(500);
	}

	std::string found = "[+] Found " + modulename + " at";
	Log(found, PVOID(mem));
	Log("\n", NULL);

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

	Sleep(1000);

	Log("\nWaiting for csgo.exe!\n", NULL);

	Global->PID = Kernel.GetProcessPID("csgo.exe");
	while (!Global->PID)
	{
		Global->PID = Kernel.GetProcessPID("csgo.exe");
		Sleep(500);
	}

	Log("PID", Global->PID);
	Kernel.SetupPid(Global->PID); //setupped pid!
	Log("\n", NULL);


	WaitModule(Global->Client, "client.dll");
	WaitModule(Global->Engine, "engine.dll");


	//Global->K = &Kernel; //Setup driver object
	Log("[+] Sucessfully settupped Kernel object!\n\n", NULL);

	//ofs->getOffsets();


	//Log("Game Base", (PVOID)Game::Base);
	//Log("LocalPlayer", (PVOID)Game::GetLocalPlayer());
	//Log("Health", (PVOID)(Game::GetLocalPlayer() + OFF_HEALTH));

	//auto OldHealth = Game::GetHealth();

	//while (true)
	//{
	//	//uint32_t CurHealth = Game::GetHealth();


	//	Kernel.WriteMemory<int>(Game::GetHealth(true), 999);


	//	Sleep(500);
	//}

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
