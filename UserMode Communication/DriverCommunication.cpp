//#include "Utils/Utils.h"
//#include "DriverCommunication.h"
//
//DriverCommunication Driver;
//
//template<typename ... A>
//uint64_t DriverCommunication::CallHook(const A ... arguments)
//{
//	std::call_once(flag, [] { LoadLibrary("user32.dll"); });
//	void* bControlFunction = GetProcAddress(LoadLibrary("win32u.dll"), "NtOpenCompositionSurfaceSectionInfo");
//	const auto bControl = static_cast<uint64_t(__stdcall*)(A...)>(bControlFunction);
//	return bControl(arguments ...);
//}
//
//template <typename type>
//void DriverCommunication::WriteMemory(ULONG64 uAdress, type uBuffer, HANDLE uPid, ULONG uSize)
//{
//	MEMORYCOMMUNICATION m{};
//	m.kFunction = WRITE;
//	m.kPid = uPid;
//	m.kAddress = uAdress;
//	m.kBuffer = &uBuffer;
//	m.kSize = uSize;
//	m.kModuleName = NULL;
//	m.KProcessName = NULL;
//
//	CallHook(&m);
//}
//
//template <typename type>
//type DriverCommunication::ReadMemory(ULONG64 uAddress, HANDLE uPid)
//{
//	type uBuffer{};
//
//	MEMORYCOMMUNICATION m{};
//	m.kFunction = READ;
//	m.kPid = uPid;
//	m.kAddress = uAddress;
//	m.kBuffer = &uBuffer;
//	m.kSize = sizeof(type);
//	m.kModuleName = NULL;
//	m.KProcessName = NULL;
//
//	CallHook(&m);
//	return uBuffer;
//}
//
//uintptr_t DriverCommunication::GetModuleBaseAdress(const char* uModuleName, HANDLE uPid)
//{
//	MEMORYCOMMUNICATION m{};
//	m.kFunction = GETMODULEBASE;
//	m.kPid = uPid;
//	m.kAddress = NULL;
//	m.kBuffer = NULL;
//	m.kSize = NULL;
//	m.kModuleName = NULL;
//	m.KProcessName = NULL;
//
//	CallHook(&m);
//
//	return (uintptr_t)m.kBuffer;
//}
//
//uintptr_t DriverCommunication::GetProcessPeb(HANDLE uPid)
//{
//	MEMORYCOMMUNICATION m{};
//	m.kFunction = GETPROCESSPEB;
//	m.kPid = uPid;
//	m.kAddress = NULL;
//	m.kBuffer = NULL;
//	m.kSize = NULL;
//	m.kModuleName = NULL;
//	m.KProcessName = NULL;
//
//	CallHook(&m);
//
//	return (uintptr_t)m.kBuffer;
//}
//
//HANDLE DriverCommunication::GetProcessPID(const char* uProcessName)
//{
//	MEMORYCOMMUNICATION m{};
//	m.kFunction = GETPID;
//	m.kPid = NULL;
//	m.kAddress = NULL;
//	m.kBuffer = NULL;
//	m.kSize = NULL;
//	m.kModuleName = NULL;
//	m.KProcessName = uProcessName;
//
//	CallHook(&m);
//
//	return m.kPid;
//}
//
//void DriverCommunication::SetupPid(HANDLE DefaultPID)
//{
//	std::cout << "Sucessfully Setupped PID: " << dPid << "\n";
//	this->dPid = DefaultPID;
//	this->uSetupped = true;
//}
//
//template <typename type>
//bool DriverCommunication::Write(ULONG64 uAdress, type uBuffer, HANDLE uPid )
//{
//	if (!uAdress || !uBuffer) {
//		std::cout << "Invalid Adress or Buffer while writing!\n";
//		return false;
//	}
//
//	if (!uPid && this->uSetupped) {
//		uPid = this->dPid;
//	}
//	else{
//		std::cout << "Invalid PID while writing!\n";
//		return false;
//	}
//
//	if (!Utils::Get().IsProcessRunning((DWORD)uPid)){
//		std::cout << "Can't find process while writing!\n";
//		return false;
//	}
//
//
//}