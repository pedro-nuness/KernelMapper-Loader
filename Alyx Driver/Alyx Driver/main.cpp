#include "Utils/memory.h"
#include "Utils/imports.h"
#include "Utils/cleaner.h"


COPY_MEMORY NULLMem{ 0, nullptr, false, NULL, NULL, NULL, nullptr, nullptr };

void hook_handler(PVOID called_param)
{
	if (ExGetPreviousMode() != UserMode) {
		DbgPrintEx(0, 0, "Ignored call\n");
		return;
	}

	if (!called_param) {
		DbgPrintEx(0, 0, "Invalid ptr!\n");
		return;
	}

	COPY_MEMORY* Mem = (COPY_MEMORY*)called_param;
	PEPROCESS process = NULL;

	if (Mem != nullptr) 
	{
		Mem->kSucess = false;

		switch (Mem->kFunction)
		{
		case GETPID:

			DbgPrintEx(0, 0, "GetPID called!\n");
			if (Mem->KProcessName) {
				Mem->kPid = memory::get_process_id(Mem->KProcessName);
			}

			if (Mem->kPid) {
				Mem->kSucess = true;
			}

			break;
		case GETMODULEBASE:
			DbgPrintEx(0, 0, "GetModuleBase called!\n");
			if (NT_SUCCESS(PsLookupProcessByProcessId(Mem->kPid, &process))) {
				Mem->kBuffer = (void*)memory::get_module_base_x64(process);
			}

			if (Mem->kBuffer) {
				Mem->kSucess = true;
			}

			break;
		case GETPROCESSPEB:
			DbgPrintEx(0, 0, "GetProcessPED called!\n");
			if (NT_SUCCESS(PsLookupProcessByProcessId(Mem->kPid, &process))) {
				Mem->kBuffer = (void*)PsGetProcessPeb(process);
			}

			if (Mem->kBuffer) {
				Mem->kSucess = true;
			}
			break;

		case READ:
			DbgPrintEx(0, 0, "ReadMemory called!\n");
			if (Mem->kPid && Mem->kAddress && Mem->kBuffer && Mem->kSize)
				Mem->kSucess = memory::read_kernel_memory(Mem->kPid, (PVOID)Mem->kAddress, Mem->kBuffer, Mem->kSize);
			break;
		case WRITE:
			DbgPrintEx(0, 0, "WriteMemory called!\n");
			if (Mem->kPid && Mem->kBuffer && Mem->kAddress && Mem->kSize)
				Mem->kSucess = memory::write_kernel_memory(Mem->kPid, Mem->kBuffer, (PVOID)Mem->kAddress, Mem->kSize);
			break;
		}
	}

}

void bInitialize()
{
	if (memory::call_kernel_function(&hook_handler))
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