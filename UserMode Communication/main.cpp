#include <iostream>
#include "DriverCommunication.h"


DriverCommunication Kernel;

int main() {

	std::string process;

	std::cout << "Target process: ";
	std::cin >> process;
	std::cout << "\n";

	if (process.empty())
	{
		std::cout << "Invalid Process!\n";
		Sleep(2000);
		exit(0);
	}

	auto PID = Kernel.GetPID(process.c_str());
	if (!PID)
	{
		std::cout << "Cannot get PID!\n";
		Sleep(2000);
		exit(0);
	}
	
	std::cout << "PID: " << PID << "\n";

	auto ModuleBase = Kernel.GetModuleBase(process.c_str(), PID);
	if(!ModuleBase)
	{
		std::cout << "Failed to get ModuleBase!\n";
		Sleep(2000);
		exit(0);
	}

	std::cout << "ModBase: " << std::hex << ModuleBase << std::dec << std::endl;
	system("pause");

	return 1;
}
