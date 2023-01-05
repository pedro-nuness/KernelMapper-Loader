#include "Utils/memory.h"
#include "Utils/memory.h"
#include "Utils/cleaner.h"
#include "..\..\Kernel-User.h"

void hook_handler(PVOID called_param)
{

	if (ExGetPreviousMode() != UserMode)
	{
		DbgPrintEx(0, 0, "Ignored call\n");
	}
	else
	{
		if (!called_param)
		{
			DbgPrintEx(0, 0, "Invalid ptr!\n");
		}
		else
		{
			COPY_MEMORY* Mem = (COPY_MEMORY*)called_param;
			PEPROCESS process = NULL;
			void* ptr = nullptr;

			if (Mem != nullptr)
			{
				bool sucess = false;

				switch (Mem->kFunction)
				{
				case GETPID:
					//DbgPrintEx(0, 0, "GetPID called!\n");
					if (Mem->KProcessName) {
						Mem->kPid = memory::GetProcessID(Mem->KProcessName);
					}

					if (Mem->kAnswer->WantsAnswer()) {
						Mem->kAnswer->SetAnswer("Called GETPID!");
						Mem->kAnswer->SetStatus(Mem->kPid ? true : false);
					}
					//if (Mem->GetAnswer)
						//GetFullText(Mem->kResponse, "Called GetPID!\n");

					break;
				case GETMODULEBASE:
					DbgPrintEx(0, 0, "GetModuleBase called!");

					Mem->kBuffer = nullptr;
					sucess = false;

					if (Mem->kModuleName && Mem->kPid)
					{			
						ptr = memory::GetBase(Mem->kPid, Mem->kModuleName);
						if (ptr != nullptr) {
							Mem->kBuffer = ptr;
							sucess = true;
						}		
					}

					if (Mem->kAnswer->WantsAnswer()) {
						Mem->kAnswer->SetAnswer("Called GetModuleBase!");
						Mem->kAnswer->SetStatus(sucess ? true : false);
					}
					
					//if (Mem->)
						//GetFullText(Mem->kResponse, "Called ModuleBase!\n");

					break;
				case GETPROCESSPEB:
					DbgPrintEx(0, 0, "GetProcessPED called!");
					
					Mem->kBuffer = nullptr;
					sucess = false;

					if ( Mem->kPid)
					{
						ptr = memory::GetProcessPEB(Mem->kPid);
						if (ptr != nullptr) {
							Mem->kBuffer = ptr;
							sucess = true;
						}
					}


					if (Mem->kAnswer->WantsAnswer()) {
						Mem->kAnswer->SetAnswer("Called GetProcessPEB!");
						Mem->kAnswer->SetStatus(Mem->kBuffer ? true : false);
					}

					//GetFullText(Mem->kResponse, "Called ProcessPeb!\n");

					break;

				case READ:
					DbgPrintEx(0, 0, "ReadMemory called!");


					if (Mem->kPid && Mem->kAddress && Mem->kBuffer && Mem->kSize)
						sucess = memory::ReadMemory(Mem->kPid, (PVOID)Mem->kAddress, Mem->kBuffer, Mem->kSize);

					if (Mem->kAnswer->WantsAnswer()) {
						Mem->kAnswer->SetAnswer("Called ReadMemory!");
						Mem->kAnswer->SetStatus(sucess);
					}

					break;
				case WRITE:
					DbgPrintEx(0, 0, "WriteMemory called!");

					if (Mem->kPid && Mem->kBuffer && Mem->kAddress && Mem->kSize)
						sucess = memory::WriteMemory(Mem->kPid, Mem->kBuffer, (PVOID)Mem->kAddress, Mem->kSize);

					if (Mem->kAnswer->WantsAnswer()) {
						Mem->kAnswer->SetAnswer("Called WriteMemory!");
						Mem->kAnswer->SetStatus(sucess);
					}
					break;
				case COMMUNICATION:

					if (Mem->kAnswer->WantsAnswer()) {
						Mem->kAnswer->SetAnswer("Driver Is Alive!");
						Mem->kAnswer->SetStatus(true);
					}

					//GetFullText(Mem->kResponse, "Hello World!");
					break;
				default:
					Mem->kAnswer->SetAnswer("Invalid input!");
					Mem->kAnswer->SetStatus(false);
					break;
				}
			}
		}

	}
}

void bInitialize()
{
	if (memory::Hook(&hook_handler))
		DbgPrintEx(0, 0, "Hooked handler.\n");
	else
		DbgPrintEx(0, 0, "Failed to hook handler!.\n");
}


extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObj, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObj);
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrintEx(0, 0, "DriverEntry!\n");
	bInitialize();

	return STATUS_SUCCESS;
}