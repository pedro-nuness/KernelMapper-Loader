#include "kdmapper.hpp"

HANDLE iqvw64e_device_handle;

LONG WINAPI SimplestCrashHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
	if (ExceptionInfo && ExceptionInfo->ExceptionRecord)
		Log(L"[!!] Crash at addr 0x" << ExceptionInfo->ExceptionRecord->ExceptionAddress << L" by 0x" << std::hex << ExceptionInfo->ExceptionRecord->ExceptionCode << std::endl);
	else
		Log(L"[!!] Crash" << std::endl);

	if (iqvw64e_device_handle)
		intel_driver::Unload(iqvw64e_device_handle);

	return EXCEPTION_EXECUTE_HANDLER;
}

int paramExists(const int argc, wchar_t** argv, const wchar_t* param) {
	size_t plen = wcslen(param);
	for (int i = 1; i < argc; i++) {
		if (wcslen(argv[i]) == plen + 1ull && _wcsicmp(&argv[i][1], param) == 0 && argv[i][0] == '/') { // with slash
			return i;
		}
		else if (wcslen(argv[i]) == plen + 2ull && _wcsicmp(&argv[i][2], param) == 0 && argv[i][0] == '-' && argv[i][1] == '-') { // with double dash
			return i;
		}
	}
	return -1;
}

void help() {
	Log(L"\r\n\r\n[!] Incorrect Usage!" << std::endl);
	Log(L"[+] Usage: kdmapper.exe [--free][--mdl][--PassAllocationPtr] driver" << std::endl);
}

bool callbackExample(ULONG64* param1, ULONG64* param2, ULONG64 allocationPtr, ULONG64 allocationSize, ULONG64 mdlptr) {
	UNREFERENCED_PARAMETER(param1);
	UNREFERENCED_PARAMETER(param2);
	UNREFERENCED_PARAMETER(allocationPtr);
	UNREFERENCED_PARAMETER(allocationSize);
	UNREFERENCED_PARAMETER(mdlptr);
	Log("[+] Callback example called" << std::endl);
	
	/*
	This callback occurs before call driver entry and
	can be usefull to pass more customized params in 
	the last step of the mapping procedure since you 
	know now the mapping address and other things
	*/
	return true;
}

std::wstring StringToWstring(std::string str) {
	std::string t(str);
	std::wstring name(t.begin(), t.end());
	return name;
}

bool MapDriver(std::string DriverPath, bool MdlMode = false, bool Free = false, bool PassAlocationPtr = false)
{
	SetUnhandledExceptionFilter(SimplestCrashHandler);

	if (Free) {
		Log(L"[+] Free pool memory after usage enabled" << std::endl);
	}

	if (MdlMode) {
		Log(L"[+] Mdl memory usage enabled" << std::endl);
	}

	if (PassAlocationPtr) {
		Log(L"[+] Pass Allocation Ptr as first param enabled" << std::endl);
	}


	if (!std::filesystem::exists(DriverPath)) {
		Log(L"[-] File " << StringToWstring(DriverPath) << L" doesn't exist" << std::endl);
		return false;
	}

	iqvw64e_device_handle = intel_driver::Load();

	if (iqvw64e_device_handle == INVALID_HANDLE_VALUE)
		return false;

	std::vector<uint8_t> raw_image = { 0 };
	if (!utils::ReadFileToMemory(StringToWstring(DriverPath), &raw_image)) {
		Log(L"[-] Failed to read image to memory" << std::endl);
		intel_driver::Unload(iqvw64e_device_handle);
		return false;
	}

	NTSTATUS exitCode = 0;
	if (!kdmapper::MapDriver(iqvw64e_device_handle, raw_image.data(), 0, 0, Free, true, MdlMode, PassAlocationPtr, callbackExample, &exitCode)) {
		Log(L"[-] Failed to map " << StringToWstring(DriverPath) << std::endl);
		intel_driver::Unload(iqvw64e_device_handle);
		return false;
	}

	if (!intel_driver::Unload(iqvw64e_device_handle)) {
		Log(L"[-] Warning failed to fully unload vulnerable driver " << std::endl);
	}
	Log(L"[+] success" << std::endl);

	return true;
}