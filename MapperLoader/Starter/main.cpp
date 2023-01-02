#include <iostream>
#include <windows.h>
#include <vector>

#include <d3d9.h>

bool doOnce = false;

bool show_login = true;
bool show_register = false;
bool show_logged = false;

#include "auth.hpp"
#include <string>
#include "skStr.h"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <Wininet.h>
#include <string>
#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>
#include <filesystem>
#include <random>

#include "json.hpp"
#include <TlHelp32.h>


using json = nlohmann::json;

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")

using namespace KeyAuth;

std::string name = ("DayzCheat"); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = ("OBz74zBgIX"); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = ("432f0279997bf737f1581324515a5b923d0bcb73d95ca5f47f0399248b037967"); // app secret, the blurred text on licenses tab and other tabs
std::string version = ("1.0"); // leave alone unless you've changed version on website
std::string url = ("https://keyauth.win/api/1.2/"); // change if you're self-hosting

api KeyAuthApp(name, ownerid, secret, version, url);

bool CheckStrings(std::string bString1, std::string bExpectedResult)
{
	size_t found = bString1.find(bExpectedResult);
	if (found != std::string::npos)
	{
		return true;
	}

	return false;
}

void CreateF(api* app, std::string path, std::string FileID)
{
	std::vector<std::uint8_t> bytes = KeyAuthApp.download(FileID);

	if (!KeyAuthApp.data.success) // check whether file downloaded correctly
	{
		MessageBox(NULL, KeyAuthApp.data.message.c_str(), std::string(("Warning!")).c_str(), MB_OK);
		exit(0);
	}

	std::ofstream file(path, std::ios_base::out | std::ios_base::binary);
	file.write((char*)bytes.data(), bytes.size());
	file.close();
}

#define	kdu std::string(("428502"))
#define drv64 std::string(("851567"))
#define driver std::string(("446426"))
#define DLL std::string(("610673"))
#define Starter std::string(("384969"))

bool Valid = false;

DWORD GetProcessPID(LPCTSTR ProcessName) // non-conflicting function name
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) { // must call this first
		do {
			if (!lstrcmpi(pt.szExeFile, ProcessName)) {
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap); // close handle on failure
	return 0;
}

BOOL IsProcessRunning(DWORD pid)
{
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
	DWORD ret = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return ret == WAIT_TIMEOUT;
}

std::string GetExeFileName()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}

std::string GetExePath()
{
	std::string f = GetExeFileName();
	return f.substr(0, f.find_last_of("\\/"));
}

void Download(std::string path, std::string url) {
	URLDownloadToFile(NULL, url.c_str(), path.c_str(), 0, NULL);
}

bool ReadFileToMemory(const std::string& file_path, std::vector<uint8_t>* out_buffer)
{
	std::ifstream file_ifstream(file_path, std::ios::binary);

	if (!file_ifstream)
		return false;

	out_buffer->assign((std::istreambuf_iterator<char>(file_ifstream)), std::istreambuf_iterator<char>());
	file_ifstream.close();

	return true;
}

bool CheckAutenticity(api* App)
{
	CreateF(App, ("c:\\tmp\\og.exe"), Starter);
	std::vector<uint8_t> VerifiedFile;
	ReadFileToMemory(("c:\\tmp\\og.exe"), &VerifiedFile);

	auto CurrentPath = GetExePath() + GetExeFileName();

	std::vector<uint8_t> ThisFile;
	ReadFileToMemory(CurrentPath, &ThisFile);

	if (ThisFile != VerifiedFile)
	{
		exit(0);
		return false;
	}

	Valid = true;
	return true;
}


int main()
{
	std::cout << skCrypt("\n\n Connecting...\n");
	KeyAuthApp.init();
	if (!KeyAuthApp.data.success)
	{
		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
		Sleep(1500);
		exit(0);
	}

	if (KeyAuthApp.checkblack())
		abort();

	auto CommunicationFolder = std::string(("c:\\Windows\\Communication\\"));

	std::string JsonFile;

	std::cout << ("[+] Reading files on current path!\n");

	for (const auto& entry : std::filesystem::recursive_directory_iterator(CommunicationFolder))
	{
		if (CheckStrings(entry.path().string(), (".json")))
		{
			JsonFile = entry.path().string();
			break;
		}
	}

	if (JsonFile.empty())
		exit(0);

	std::cout << ("[+] Found json file!\n");

	std::ifstream i(JsonFile);
	json j;
	i >> j;
	i.close();
	Sleep(500);

	std::cout << ("[+] Sucessfully read json file\n");

	remove(JsonFile.c_str());
	std::cout << ("[+] Attemping login!\n");

	KeyAuthApp.login(j["user"], j["password"]);

	if (!KeyAuthApp.data.success)
	{
		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message << std::endl;
		Sleep(1500);
		exit(0);
	}
	std::cout << ("[+] Logged In!\n");

	auto random = std::string(j["random"]);

	Sleep(1000);

	std::cout << ("[+] Communicating with loader!\n");

	json response;
	response["pid"] = GetCurrentProcessId();
	std::ofstream outfile(CommunicationFolder + ("ready.json"));
	outfile << j;
	outfile.close();

	std::cout << ("[+] Waiting loader response!\n");

	while (true)
	{
		if (!IsProcessRunning(j["loaderpid"]))
		{
			exit(0);
		}

		if (IsDebuggerPresent())
		{
			exit(0);
		}

		Sleep(2000);
	}

	return 1;
}