#pragma once
//#include "Kernel-User.h"
#include <cstdint>
#include <memory>
#include <string_view>
#include <windows.h>
#include <TlHelp32.h>
#include <mutex>
#include <iostream>
#include <vector>
#include <cassert>
#include "..\Kernel-User.h"

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
	AnswareManager WriteMemory(ULONG64 uAdress, type uBuffer, HANDLE uPid)
	{
		AnswareManager DefaultAnswer = AnswareManager(true);

		ULONG uSize = sizeof(uBuffer);

		_COPY_MEMORY m{  };
		m.kFunction = WRITE;
		m.kPid = uPid;
		m.kAddress = uAdress;
		m.kBuffer = &uBuffer;
		m.kSize = uSize;
		m.kModuleName = NULL;
		m.KProcessName = NULL;
		m.kAnswer = &DefaultAnswer;

		CallHook(&m);

		return DefaultAnswer;
	}

	template <typename type>
	type ReadMemory(ULONG64 uAddress, HANDLE uPid, AnswareManager* answer = nullptr)
	{
		type uBuffer{};

		AnswareManager DefaultAnswer = AnswareManager(true);

		_COPY_MEMORY m{ };
		m.kFunction = READ;
		m.kPid = uPid;
		m.kAddress = uAddress;
		m.kBuffer = &uBuffer;
		m.kSize = sizeof(type);
		m.kModuleName = NULL;
		m.KProcessName = NULL;
		m.kAnswer = &DefaultAnswer;

		CallHook(&m);

		if (answer != nullptr)
			*answer = DefaultAnswer;

		return uBuffer;
	}

	void* GetModuleBaseAdress(const char* uModuleName, HANDLE uPid, AnswareManager* answer = nullptr)
	{
		AnswareManager DefaultAnswer = AnswareManager(true);

		_COPY_MEMORY m{ };
		m.kFunction = GETMODULEBASE;
		m.kPid = uPid;
		m.kAddress = NULL;
		m.kBuffer = NULL;
		m.kSize = NULL;
		m.kModuleName = uModuleName;
		m.KProcessName = NULL;
		m.kAnswer = &DefaultAnswer;

		CallHook(&m);

		if (answer != nullptr)
			*answer = DefaultAnswer;

		return m.kBuffer;
	}

	uintptr_t GetProcessPeb(HANDLE uPid, AnswareManager* answer = nullptr)
	{
		AnswareManager DefaultAnswer = AnswareManager(true);

		_COPY_MEMORY m{};
		m.kFunction = GETPROCESSPEB;
		m.kPid = uPid;
		m.kAddress = NULL;
		m.kBuffer = NULL;
		m.kSize = NULL;
		m.kModuleName = NULL;
		m.KProcessName = NULL;
		m.kAnswer = &DefaultAnswer;

		CallHook(&m);

		if (answer != nullptr)
			*answer = DefaultAnswer;

		return (uintptr_t)m.kBuffer;
	}

	HANDLE GetProcessPID(const char* uProcessName, AnswareManager* answer = nullptr)
	{
		AnswareManager DefaultAnswer = AnswareManager(true);

		_COPY_MEMORY m{  };
		m.kFunction = GETPID;
		m.kPid = NULL;
		m.kAddress = NULL;
		m.kBuffer = NULL;
		m.kSize = NULL;
		m.kModuleName = NULL;
		m.KProcessName = uProcessName;
		m.kAnswer = &DefaultAnswer;

		CallHook(&m);

		if (answer != nullptr)
			*answer = DefaultAnswer;

		return m.kPid;
	}

	AnswareManager CheckDriver()
	{
		AnswareManager DefaultAnswer = AnswareManager(true);

		_COPY_MEMORY m{};
		m.kFunction = COMMUNICATION;
		m.kPid = NULL;
		m.kAddress = NULL;
		m.kBuffer = nullptr;
		m.kSize = NULL;
		m.kModuleName = NULL;
		m.KProcessName = NULL;
		m.kAnswer = &DefaultAnswer;

		CallHook(&m);

		return DefaultAnswer;
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