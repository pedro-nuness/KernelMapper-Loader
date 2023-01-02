#pragma once
#include <cstdint>
#include <memory>
#include <string_view>
#include <windows.h>
#include <TlHelp32.h>
#include <mutex>
#include <iostream>
#include <vector>
#include <cassert>

enum FUNCTIONS
{
	WRITE = 1,
	READ = 2,
	GETPID = 3,
	GETMODULEBASE = 4,
	GETPROCESSPEB = 5
};

typedef struct _MEMORYCOMMUNICATION
{
	int kFunction;
	void* kBuffer;
	bool kSucess;

	ULONG64		kAddress;
	ULONG		kSize;
	HANDLE		kPid;

	const char* kModuleName;
	const char* KProcessName;

}MEMORYCOMMUNICATION;


class DriverCommunication
{
	std::once_flag flag;

	template<typename ... A>
	uint64_t CallHook(const A ... arguments)
	{
		std::call_once(flag, [] { LoadLibrary("user32.dll"); });
		void* control_function = GetProcAddress(LoadLibrary("win32u.dll"), "NtOpenCompositionSurfaceSectionInfo");
		const auto control = static_cast<uint64_t(__stdcall*)(A...)>(control_function);
		return control(arguments ...);
	}

	template <typename type>
	void WriteMemory(ULONG64 uAdress, type uBuffer, HANDLE uPid)
	{
		ULONG uSize = sizeof(uBuffer);

		MEMORYCOMMUNICATION m{};
		m.kFunction = WRITE;
		m.kPid = uPid;
		m.kAddress = uAdress;
		m.kBuffer = &uBuffer;
		m.kSize = uSize;
		m.kModuleName = NULL;
		m.KProcessName = NULL;
		m.kSucess = false;

		CallHook(&m);

		return m.kSucess;
	}

	template <typename type>
	type ReadMemory(ULONG64 uAddress, HANDLE uPid)
	{
		type uBuffer{};

		MEMORYCOMMUNICATION m{};
		m.kFunction = READ;
		m.kPid = uPid;
		m.kAddress = uAddress;
		m.kBuffer = &uBuffer;
		m.kSize = sizeof(type);
		m.kModuleName = NULL;
		m.KProcessName = NULL;

		CallHook(&m);
		return uBuffer;
	}

	uintptr_t GetModuleBaseAdress(const char* uModuleName, HANDLE uPid)
	{
		MEMORYCOMMUNICATION m{};
		m.kFunction = GETMODULEBASE;
		m.kPid = uPid;
		m.kAddress = NULL;
		m.kBuffer = NULL;
		m.kSize = NULL;
		m.kModuleName = uModuleName;
		m.KProcessName = NULL;

		CallHook(&m);

		return (uintptr_t)m.kBuffer;
	}

	uintptr_t GetProcessPeb(HANDLE uPid)
	{
		MEMORYCOMMUNICATION m{};
		m.kFunction = GETPROCESSPEB;
		m.kPid = uPid;
		m.kAddress = NULL;
		m.kBuffer = NULL;
		m.kSize = NULL;
		m.kModuleName = NULL;
		m.KProcessName = NULL;

		CallHook(&m);

		return (uintptr_t)m.kBuffer;
	}

	HANDLE GetProcessPID(const char* uProcessName)
	{
		MEMORYCOMMUNICATION m{};
		m.kFunction = GETPID;
		m.kPid = NULL;
		m.kAddress = NULL;
		m.kBuffer = NULL;
		m.kSize = NULL;
		m.kModuleName = NULL;
		m.KProcessName = uProcessName;

		CallHook(&m);

		return m.kPid;
	}

	template <typename type>
	bool FAIL(type t1, std::string message, bool inv = false)
	{
		if (inv && t1)
		{
			std::cout << message;
			return false;
		}
		else if (!t1)
		{
			std::cout << message;
				return false;
		}

		return true;
	}



	HANDLE dPid = NULL;
	bool uSetupped = false;

	HANDLE GetPid(HANDLE PassedPID)
	{
		if (PassedPID)
			return PassedPID;
		else if (!this->uSetupped)
			return NULL;
		else
			return this->dPid;
	}


public:

	void SetupPid(HANDLE DefaultPID)
	{
		std::cout << "Sucessfully Setupped PID: " << dPid << "\n";
		this->dPid = DefaultPID;
		this->uSetupped = true;
	}


	template <typename type>
	bool Write(ULONG64 uAdress, type uBuffer, HANDLE uPid = 0)
	{

		if (FAIL(uAdress, "Invalid Adress while writing!\n"))
			return false;
		if (FAIL(uBuffer, "Invalid Buffer while writing!\n"))
			return false;

		uPid = this->GetPid(uPid);
		if (FAIL(uPid, "Invalid PID while reading!"))
			return false;

		Communicate(WRITE, uBuffer, uAdress, sizeof(uBuffer), uPid, NULL, NULL);

		return true;
	}


	template <typename type>
	type Read(ULONG64 uAdress, HANDLE uPid = 0)
	{
		if (FAIL(uAdress, "Invalid Adress while reading!\n"))
			return false;

		uPid = this->GetPid(uPid);
		if (FAIL(uPid, "Invalid PID while reading!"))
			return false;

		type uBuffer;

		return ReadMemory(uAdress, uPid);
	}

	MEMORYCOMMUNICATION m{};

	uintptr_t GetModuleBase(const char* uModuleName, HANDLE uPid = 0)
	{
		if (FAIL(uModuleName, "Invalid ModuleName while getting base!\n"))
			return NULL;

		uPid = this->GetPid(uPid);
		if (FAIL(uPid, "Invalid PID while reading!\n"))
			return NULL;

		return GetModuleBaseAdress(uModuleName, uPid);
	}

	HANDLE GetPID(const char* uProcessName)
	{
		if (FAIL(uProcessName, "Invalid process name while getting pid!\n"))
			return NULL;

		return GetProcessPID(uProcessName);
	}

	uintptr_t GetPEB(HANDLE uPid)
	{
		uPid = this->GetPid(uPid);
		if (FAIL(uPid, "Invalid PID while getting peb!\n"))
			return NULL;

		return GetProcessPeb(uPid);
	}


};


//
//namespace driver
//{
//
//
//	static HANDLE get_process_id(const char* process_name) {
//		COPY_MEMORY m{};
//		m.get_pid = true;
//		m.process_name = process_name;
//		call_hook(&m);
//
//		return m.pid;
//	}
//
//	static uintptr_t get_module_base_address(const char* module_name)
//	{
//		COPY_MEMORY m{};
//		m.base = true;
//		m.pid = sdk::process_id;
//		m.module_name = module_name;
//		call_hook(&m);
//
//		return (uintptr_t)m.buffer;
//	}
//
//	static uintptr_t get_peb()
//	{
//		COPY_MEMORY m{};
//		m.peb = true;
//		m.pid = sdk::process_id;
//		call_hook(&m);
//		return (uintptr_t)m.buffer;
//	}
//
//	template <typename type>
//	type read(ULONG64 address)
//	{
//		type buffer{};
//
//		COPY_MEMORY m{};
//		m.read = true;
//		m.pid = sdk::process_id;
//		m.address = address;
//		m.buffer = &buffer;
//		m.size = sizeof(type);
//
//		call_hook(&m);
//		return buffer;
//	}
//
//	template <typename type>
//	void write(ULONG64 address, type value, ULONG size = sizeof(value))
//	{
//		COPY_MEMORY m{};
//		m.write = true;
//		m.pid = sdk::process_id;
//		m.address = address;
//		m.buffer = &value;
//		m.size = size;
//
//		call_hook(&m);
//	}
//
//}