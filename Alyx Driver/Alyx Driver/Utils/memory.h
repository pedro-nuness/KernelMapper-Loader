#pragma once
#include "imports.h"
#include <ntstrsafe.h>
#include <windef.h>


namespace memory {

	PVOID GetSystemModuleBase(const char* module_name) {
		ULONG bytes = 0;
		NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);

		if (!bytes)
			return 0;

		PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, 0x454E4F45); // 'ENON'

		status = ZwQuerySystemInformation(SystemModuleInformation, modules, bytes, &bytes);

		if (!NT_SUCCESS(status))
			return 0;

		PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
		PVOID module_base = 0, module_size = 0;

		for (ULONG i = 0; i < modules->NumberOfModules; i++)
		{
			if (strcmp((char*)module[i].FullPathName, module_name) == 0)
			{
				module_base = module[i].ImageBase;
				module_size = (PVOID)module[i].ImageSize;
				break;
			}
		}

		if (modules)
			ExFreePoolWithTag(modules, 0);

		if (module_base <= 0)
			return 0;

		return module_base;
	}

	PVOID GetProcessPEB(HANDLE pid)
	{
		if (!pid)
			return 0;

		PVOID bPebAdress = 0;
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(pid, &process))) {

			BOOLEAN isWow64 = (PsGetProcessWow64Process(process) != NULL) ? TRUE : FALSE;

			if (isWow64){

				PPEB32 pPeb32 = (PPEB32)PsGetProcessWow64Process(process);
				if (pPeb32 != nullptr){
					bPebAdress = (PVOID)pPeb32;
				}
			}
			else{
				PPEB pPeb = PsGetProcessPeb(process);
				if (pPeb != nullptr){
					bPebAdress = (PVOID)pPeb;	
				}
			}

			ObfDereferenceObject(process);
			return bPebAdress;
		}
	}

	HANDLE GetProcessID(const char* process_name) {
		ULONG buffer_size = 0;
		ZwQuerySystemInformation(SystemProcessInformation, NULL, NULL, &buffer_size);

		auto buffer = ExAllocatePoolWithTag(NonPagedPool, buffer_size, 'mder');
		if (!buffer) {
			DbgPrintEx(0, 0, "failed to allocate pool (get_process_id)");
			return 0;
		}

		ANSI_STRING process_name_ansi = {};
		UNICODE_STRING process_name_unicode = {};
		RtlInitAnsiString(&process_name_ansi, process_name);
		if (!NT_SUCCESS(RtlAnsiStringToUnicodeString(&process_name_unicode, &process_name_ansi, TRUE))) 
		{
			DbgPrintEx(0, 0, "failed to convert string (get_process_id)");
			RtlFreeUnicodeString(&process_name_unicode);
			return 0;
		}

		auto process_info = (PSYSTEM_PROCESS_INFO)buffer;
		if (NT_SUCCESS(ZwQuerySystemInformation(SystemProcessInformation, process_info, buffer_size, NULL))) {
			while (process_info->NextEntryOffset) {
				if (!RtlCompareUnicodeString(&process_name_unicode, &process_info->ImageName, true)) {
					DbgPrintEx(0, 0, "process name: %wZ | process ID: %d\n", process_info->ImageName, process_info->UniqueProcessId);
					RtlFreeUnicodeString(&process_name_unicode);
					return process_info->UniqueProcessId;
				}
				process_info = (PSYSTEM_PROCESS_INFO)((BYTE*)process_info + process_info->NextEntryOffset);
			}
		}
		else {
			ExFreePoolWithTag(buffer, 'mder');
			return 0;
		}
	}

	PVOID GetSystemModuleExport(const char* module_name, LPCSTR routine_name)
	{
		PVOID lpModule = memory::GetSystemModuleBase(module_name);

		if (!lpModule)
			return NULL;

		return RtlFindExportedRoutineByName(lpModule, routine_name);
	}

	bool WriteToReadOnlyMemory(void* address, void* buffer, size_t size) {

		PMDL Mdl = IoAllocateMdl(address, size, FALSE, FALSE, NULL);

		if (!Mdl)
			return false;

		// Locking and mapping memory with RW-rights:
		MmProbeAndLockPages(Mdl, KernelMode, IoReadAccess);
		PVOID Mapping = MmMapLockedPagesSpecifyCache(Mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
		MmProtectMdlSystemAddress(Mdl, PAGE_READWRITE);

		// Write your buffer to mapping:
		RtlCopyMemory(Mapping, buffer, size);

		// Resources freeing:
		MmUnmapLockedPages(Mapping, Mdl);
		MmUnlockPages(Mdl);
		IoFreeMdl(Mdl);

		return true;
	}



	bool Hook(void* kernel_function_address) {
		if (!kernel_function_address)
			return false;

		PVOID* dxgk_routine
			= reinterpret_cast<PVOID*>(memory::GetSystemModuleExport("\\SystemRoot\\System32\\drivers\\dxgkrnl.sys", "NtOpenCompositionSurfaceSectionInfo"));

		if (!dxgk_routine) {
			return false;
		}

		BYTE dxgk_original[] = { 0x4C, 0x8B, 0xDC, 0x49, 0x89, 0x5B, 0x18, 0x4D, 0x89, 0x4B, 0x20, 0x49, 0x89, 0x4B, 0x08 };

		BYTE shell_code_start[]
		{
			0x48, 0xB8 // mov rax, [xxx]
		};

		BYTE shell_code_end[]
		{
			0xFF, 0xE0, // jmp rax
			0xCC //
		};

		RtlSecureZeroMemory(&dxgk_original, sizeof(dxgk_original));
		memcpy((PVOID)((ULONG_PTR)dxgk_original), &shell_code_start, sizeof(shell_code_start));
		uintptr_t test_address = reinterpret_cast<uintptr_t>(kernel_function_address);
		memcpy((PVOID)((ULONG_PTR)dxgk_original + sizeof(shell_code_start)), &test_address, sizeof(void*));
		memcpy((PVOID)((ULONG_PTR)dxgk_original + sizeof(shell_code_start) + sizeof(void*)), &shell_code_end, sizeof(shell_code_end));
		WriteToReadOnlyMemory(dxgk_routine, &dxgk_original, sizeof(dxgk_original));

		return true;
	}

	bool ReadMemory(HANDLE pid, PVOID address, PVOID buffer, SIZE_T size) {
		if (!address || !buffer || !size)
			return false;

		SIZE_T bytes = 0;
		PEPROCESS process;
		if (!NT_SUCCESS(PsLookupProcessByProcessId(pid, &process))) {
			DbgPrintEx(0, 0, "process lookup failed (read)");
			return false;
		}

		return MmCopyVirtualMemory(process, address, PsGetCurrentProcess(), buffer, size, KernelMode, &bytes) == STATUS_SUCCESS;
	}

	bool WriteMemory(HANDLE pid, PVOID address, PVOID buffer, SIZE_T size) {
		if (!address || !buffer || !size)
			return false;

		SIZE_T bytes = 0;
		PEPROCESS process;
		if (!NT_SUCCESS(PsLookupProcessByProcessId(pid, &process))) {
			DbgPrintEx(0, 0, "process lookup failed (write)");
			return false;
		}

		return MmCopyVirtualMemory(PsGetCurrentProcess(), address, process, buffer, size, KernelMode, &bytes) == STATUS_SUCCESS;
	}

	void SleepFor(int time) {
		int i = 0;
		while (i < time)
			i++;
	}


	PVOID BBGetUserModule(IN PEPROCESS pProcess, IN PUNICODE_STRING ModuleName)
	{
		//ASSERT(pProcess != NULL);
		if (pProcess == NULL)
			return NULL;

		 //Protect from UserMode AV
		__try
		{
			LARGE_INTEGER time = { 0 };
			time.QuadPart = -250ll * 10 * 1000;     // 250 msec.

			BOOLEAN isWow64 = (PsGetProcessWow64Process(pProcess) != NULL) ? TRUE : FALSE;
			
			// Wow64 process
			if (isWow64)
			{
				PPEB32 pPeb32 = (PPEB32)PsGetProcessWow64Process(pProcess);
				if (pPeb32 == NULL)
				{			
					return NULL;
				}

				// Wait for loader a bit
				for (INT i = 0; !pPeb32->Ldr && i < 10; i++)
				{
					//DPRINT("BlackBone: %s: Loader not intialiezd, waiting\n", __FUNCTION__);
					KeDelayExecutionThread(KernelMode, TRUE, &time);
				}

				// Still no loader
				if (!pPeb32->Ldr)
				{	
					return NULL;
				}

				// Search in InLoadOrderModuleList
				for (PLIST_ENTRY32 pListEntry = (PLIST_ENTRY32)((PPEB_LDR_DATA32)pPeb32->Ldr)->InLoadOrderModuleList.Flink;
					pListEntry != &((PPEB_LDR_DATA32)pPeb32->Ldr)->InLoadOrderModuleList;
					pListEntry = (PLIST_ENTRY32)pListEntry->Flink)
				{
					UNICODE_STRING ustr;
					PLDR_DATA_TABLE_ENTRY32 pEntry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);

					RtlUnicodeStringInit(&ustr, (PWCH)pEntry->BaseDllName.Buffer);

					if (RtlCompareUnicodeString(&ustr, ModuleName, TRUE) == 0)
						return (PVOID)pEntry->DllBase;
				}
			}
			// Native process
			else
			{
				PPEB pPeb = PsGetProcessPeb(pProcess);
				if (!pPeb)
				{
					//DPRINT("BlackBone: %s: No PEB present. Aborting\n", __FUNCTION__);
					return NULL;
				}

				// Wait for loader a bit
				for (INT i = 0; !pPeb->Ldr && i < 10; i++)
				{
					//DPRINT("BlackBone: %s: Loader not intialiezd, waiting\n", __FUNCTION__);
					KeDelayExecutionThread(KernelMode, TRUE, &time);
				}

				// Still no loader
				if (!pPeb->Ldr)
				{
					//DPRINT("BlackBone: %s: Loader was not intialiezd in time. Aborting\n", __FUNCTION__);
					return NULL;
				}

				//Search in InLoadOrderModuleList
				for (PLIST_ENTRY pListEntry = pPeb->Ldr->InLoadOrderModuleList.Flink;
					pListEntry != &pPeb->Ldr->InLoadOrderModuleList;
					pListEntry = pListEntry->Flink)
				{
					PLDR_DATA_TABLE_ENTRY pEntry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
					if (RtlCompareUnicodeString(&pEntry->BaseDllName, ModuleName, TRUE) == 0)
					return pEntry->DllBase;
				}
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			//DPRINT("BlackBone: %s: Exception, Code: 0x%X\n", __FUNCTION__, GetExceptionCode());
		}

		return NULL;
	}

	void* GetBase(HANDLE PID, const char* Module) {
		PEPROCESS process;

		if (NT_SUCCESS(PsLookupProcessByProcessId(PID, &process))) {
			
			ANSI_STRING Module_name_ansi = {};
			UNICODE_STRING Module_name_unicode = {};
			RtlInitAnsiString(&Module_name_ansi, Module);
			if (!NT_SUCCESS(RtlAnsiStringToUnicodeString(&Module_name_unicode, &Module_name_ansi, TRUE)))
			{
				DbgPrintEx(0, 0, "failed to convert string (get_process_id)");
				RtlFreeUnicodeString(&Module_name_unicode);
				return nullptr;
			}


			KeAttachProcess((PKPROCESS)process);
			void* ptr = memory::BBGetUserModule(process, &Module_name_unicode);		
			KeDetachProcess();
			ObDereferenceObject(process);
			return ptr;
		}
		return nullptr;
	}

	NTSTATUS ProtectVirtualMemory(HANDLE pid, PVOID address, ULONG size, ULONG protection, ULONG& protection_out)
	{
		if (!pid || !address || !size || !protection)
			return STATUS_INVALID_PARAMETER;

		NTSTATUS status = STATUS_SUCCESS;
		PEPROCESS target_process = nullptr;

		if (!NT_SUCCESS(PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(pid), &target_process)))
		{
			return STATUS_NOT_FOUND;
		}

		//PVOID address = reinterpret_cast<PVOID>( memory_struct->address );
		//ULONG size = (ULONG)( memory_struct->size );
		//ULONG protection = memory_struct->protection;
		ULONG protection_old = 0;

		KAPC_STATE state;
		KeStackAttachProcess(target_process, &state);

		status = ZwProtectVirtualMemory(NtCurrentProcess(), &address, &size, protection, &protection_old);

		KeUnstackDetachProcess(&state);

		if (NT_SUCCESS(status))
			protection_out = protection_old;

		ObDereferenceObject(target_process);
		return status;
	}


	void* GetProcessModuleBase(HANDLE PID, const char* modulename)
	{
		ANSI_STRING x;
		UNICODE_STRING game_module;
		RtlInitAnsiString(&x, modulename);
		RtlAnsiStringToUnicodeString(&game_module, &x, TRUE);

		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(PID, &process)))
		{
			void* base_address = nullptr;
			RtlFreeUnicodeString(&game_module);
			return base_address;
		}

		return nullptr;
	}


}