#include "Utils/memory.h"
#include "Utils/memory.h"
#include "Utils/cleaner.h"
#include "..\..\Kernel-User.h"

//// Function prototype for the target function
//typedef int (*TARGET_FUNC)(int, int);
//
//// Trampoline function
//int TrampolineFunc(int a, int b)
//{
//	// Perform any desired operations here
//
//	// Call the original target function
//	TARGET_FUNC originalFunc = (TARGET_FUNC)0x12345678;
//	int result = originalFunc(a, b);
//
//	// Perform any additional desired operations here
//
//	return result;
//}
//
//VOID TramplineHook()
//{
//	// Obtain a handle to the user-mode process
//	HANDLE hProcess = NULL;
//	NTSTATUS status = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, NULL, (HANDLE)0x1234);
//	if (!NT_SUCCESS(status))
//	{
//		// Handle the error
//		return;
//	}
//
//	// Allocate memory in the user-mode process for the trampoline function
//	PVOID pTrampoline = NULL;
//	SIZE_T trampolineSize = 4096;
//	status = ZwAllocateVirtualMemory(hProcess, &pTrampoline, 0, &trampolineSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
//	if (!NT_SUCCESS(status))
//	{
//		// Handle the error
//		ZwClose(hProcess);
//		return;
//	}
//
//	// Write the trampoline function code to the allocated memory
//	status = ZwWriteVirtualMemory(hProcess, pTrampoline, TrampolineFunc, trampolineSize, NULL);
//	if (!NT_SUCCESS(status))
//	{
//		// Handle the error
//		ZwFreeVirtualMemory(hProcess, &pTrampoline, &trampolineSize, MEM_RELEASE);
//		ZwClose(hProcess);
//		return;
//	}
//
//	// Modify the import address table (IAT) to redirect calls to the target function to the trampoline function
//	PVOID pIAT = (PVOID)0x12345678;
//	status = ZwWriteVirtualMemory(hProcess, pIAT, &pTrampoline, sizeof(pTrampoline), NULL);
//	if (!NT_SUCCESS(status))
//	{
//		// Handle the error
//		ZwFreeVirtualMemory(hProcess, &pTrampoline, &trampolineSize, MEM_RELEASE);
//		ZwClose(hProcess);
//		return;
//	}
//	// Modify the target function's code to jump to the trampoline function
//	PBYTE pTargetFunc = (PBYTE)0x12345678;
//	BYTE jumpInstruction[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
//	*(PDWORD)(jumpInstruction + 1) = (DWORD)pTrampoline - (DWORD)pTargetFunc - sizeof(jumpInstruction);
//	status = ZwWriteVirtualMemory(hProcess, pTargetFunc, jumpInstruction, sizeof(jumpInstruction), NULL);
//	if (!NT_SUCCESS(status))
//	{
//		// Handle the error
//		ZwFreeVirtualMemory(hProcess, &pTrampoline, &trampolineSize, MEM_RELEASE);
//		ZwClose(hProcess);
//		return;
//	}
//
//	// Clean up
//	ZwClose(hProcess);
//}

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
			ULONG64 adr = NULL;

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
				case GETMODULEPOINTER:
					DbgPrintEx(0, 0, "GetModuleBasePtr called!");

					Mem->kBuffer = nullptr;
					sucess = false;

					if (Mem->kModuleName && Mem->kPid)
					{			
						ptr = memory::GetModulePointer(Mem->kPid, Mem->kModuleName);
						if (ptr != nullptr) {
							Mem->kBuffer = ptr;
							sucess = true;
						}		
					}

					if (Mem->kAnswer->WantsAnswer()) {
						Mem->kAnswer->SetAnswer("Called GetModuleBasePtr!");
						Mem->kAnswer->SetStatus(sucess ? true : false);
					}
					
					//if (Mem->)
						//GetFullText(Mem->kResponse, "Called ModuleBase!\n");

					break;
				case GETMODULEADRESS:

					DbgPrintEx(0, 0, "GetModuleBaseAdr called!");

					Mem->kAddress = NULL;
					sucess = false;

					if (Mem->kModuleName && Mem->kPid)
					{
						adr = memory::GetModuleAdress(Mem->kPid, Mem->kModuleName);
						if (adr != NULL) {
							Mem->kAddress = adr;
							sucess = true;
						}
					}

					if (Mem->kAnswer->WantsAnswer()) {
						Mem->kAnswer->SetAnswer("Called GetModuleBaseAdr!");
						Mem->kAnswer->SetStatus(sucess ? true : false);
					}

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