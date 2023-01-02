
#include "menu.h"
#include "../globals.h"

#include <iostream>
#include <windows.h>
#include <vector>

#include <d3d9.h>

#include "auth.hpp"
#include <string>
#include "skStr.h"
#include "../crypt_str.h"

#include <fstream>
#include <iterator>
#include <algorithm>
#include <Wininet.h>
#include <string>
#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>
#include <filesystem>
#include <random>

#include <thread>

#include "json.hpp"
#include <TlHelp32.h>

#include "..\kdmapper\TempFiles.h"
using json = nlohmann::json;

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")

#include "..\sha1.h"

using namespace KeyAuth;

ImVec2 child_size{ 300, 276 };

bool doOnce = false;

bool show_login = true;
bool show_register = false;
bool show_logged = false;
bool bFound = false;
bool bVerified = false;

extern bool bDestroy;


int TargetPID = 0;
std::string TargetHash;

std::string name = crypt_str("DayzCheat"); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = crypt_str("OBz74zBgIX"); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = crypt_str("432f0279997bf737f1581324515a5b923d0bcb73d95ca5f47f0399248b037967"); // app secret, the blurred text on licenses tab and other tabs
std::string version = crypt_str("1.0"); // leave alone unless you've changed version on website
std::string url = crypt_str("https://keyauth.win/api/1.2/"); // change if you're self-hosting
#define	kdu std::string(crypt_str("428502"))
#define drv64 std::string(crypt_str("851567"))
#define driver std::string(crypt_str("446426"))
#define DLL std::string(crypt_str("610673"))
#define build std::string(crypt_str("262687"))

api KeyAuthApp(name, ownerid, secret, version, url);

TempFiles Dirs = SetupDirectory();
#define ResponsePath std::string(Dirs.GetCommunicationFolder() + crypt_str("ready.json"))
#define DeletePath  std::string(Dirs.GetCommunicationFolder() + crypt_str("restart.json"))
#define bDriverPath   std::string(Dirs.GetCommunicationFolder() + crypt_str("Alyx.sys"))
#define bDonePath std::string( Dirs.GetCommunicationFolder() + crypt_str("Ok.txt"))

int Inject(std::string DllPath, DWORD PID);
bool InjectBytes(std::vector<std::uint8_t> bytes, DWORD PID);
bool MapDriver(std::string DriverPath, bool MdlMode = false, bool Free = false, bool PassAlocationPtr = false);

void KillProcess(std::string process) {

	auto command = crypt_str("taskkill /f /im ") + process;
	system(command.c_str());
}
DWORD GetProcessPID(LPCTSTR ProcessName);
void End(std::string TargetName)
{
	std::cout << "END!\n";
	if (GetProcessPID(TargetName.c_str()))
		KillProcess(TargetName);
	Sleep(1000);
	remove(TargetName.c_str());
	Sleep(2000);
	exit(0);
}

class initWindow
{
public:
	const char* window_title = "Loader";
	ImVec2 window_size{ 740, 460 };

	DWORD window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse;
} iw;

void load_styles()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	{
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);

		colors[ImGuiCol_FrameBg] = ImColor(11, 11, 11, 255);
		colors[ImGuiCol_FrameBgHovered] = ImColor(11, 11, 11, 255);

		colors[ImGuiCol_Button] = ImColor(255, 0, 46, globals.button_opacity);
		colors[ImGuiCol_ButtonActive] = ImColor(255, 0, 46, globals.button_opacity);
		colors[ImGuiCol_ButtonHovered] = ImColor(255, 0, 46, globals.button_opacity);

		colors[ImGuiCol_TextDisabled] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
	}

	ImGuiStyle* style = &ImGui::GetStyle();
	{
		style->WindowPadding = ImVec2(4, 4);
		style->WindowBorderSize = 0.f;

		style->FramePadding = ImVec2(8, 6);
		style->FrameRounding = 3.f;
		style->FrameBorderSize = 1.f;
	}
}

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

bool ReadFileToMemory(const std::string& file_path, std::vector<uint8_t>* out_buffer)
{
	std::ifstream file_ifstream(file_path, std::ios::binary);

	if (!file_ifstream)
		return false;

	out_buffer->assign((std::istreambuf_iterator<char>(file_ifstream)), std::istreambuf_iterator<char>());
	file_ifstream.close();

	return true;
}

void CreateFileBytes(api* app, std::string path, std::string FileID)
{
	std::vector<std::uint8_t> bytes = KeyAuthApp.download(FileID);

	if (!KeyAuthApp.data.success) // check whether file downloaded correctly
	{
		MessageBox(NULL, KeyAuthApp.data.message.c_str(), std::string(crypt_str("Warning!")).c_str(), MB_OK);
		exit(0);
	}

	std::ofstream file(path, std::ios_base::out | std::ios_base::binary);
	file.write((char*)bytes.data(), bytes.size());
	file.close();
}

void CheckAutenticity(std::string InitializerName)
{
	while (!bFound)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	std::this_thread::sleep_for(std::chrono::seconds(2));

	while (true)
	{
		auto CurrentPID = GetProcessPID(InitializerName.c_str());

		if (!CurrentPID)
			End(InitializerName);


		if (CurrentPID != TargetPID)
		{
			End(InitializerName);
		}

		std::vector<uint8_t> CurrentBytes;
		if (!ReadFileToMemory(InitializerName, &CurrentBytes))
		{
			std::cout << crypt_str("[+] Can't read current memory!\n");
			End(InitializerName);
		}

		SHA1 sha1;
		sha1.add(CurrentBytes.data() + 0, CurrentBytes.size());
		std::string CurrentHash = sha1.getHash();

		if (CurrentHash.empty())
		{
			std::cout << crypt_str("[+] Can't get current hash!\n");
			End(InitializerName);
		}

		//std::cout << "[+] Current hash: " << CurrentHash << "\n";

		if (CurrentHash != TargetHash)
		{
			std::cout << crypt_str("[+] Sorry, unfortunately, we can't verify the current session!\n");
			End(InitializerName);
		}

		//std::cout << "[+] Hash is compatible!\n";

		if (IsDebuggerPresent())
		{
			End(InitializerName);
		}

		bVerified = true;

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	}
}



void Await(api* app, std::string InitializerName)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	TargetPID = GetProcessPID(InitializerName.c_str());
	if (!TargetPID)
	{
		std::cout << crypt_str("[+] Can't communicate with target!\n");
		End(InitializerName);
	}

	bFound = true;

	if (!std::filesystem::exists(bDonePath))
	{
		CreateFileBytes(app, bDriverPath, driver);
		Sleep(1000);

		if (!MapDriver(bDriverPath, true, true, true))
		{
			std::cout << crypt_str("[+] Couldn't load driver!\n");
			std::filesystem::remove(bDriverPath);
			End(InitializerName);
			return;
		}

		std::ofstream outfile(bDonePath); outfile.close();
		std::filesystem::remove(bDriverPath);
		MoveFileEx(bDonePath.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
	}

	while (true)
	{
		if (std::filesystem::exists(ResponsePath))
		{
			remove(ResponsePath.c_str());
			std::this_thread::sleep_for(std::chrono::seconds(1));
			auto Build = app->download(build);
			std::cout << crypt_str("[+] Waiting for safety verification!\n");
			while (!bVerified)
				std::this_thread::sleep_for(std::chrono::milliseconds(500));

			if (InjectBytes(Build, TargetPID)) 
			{
				std::cout << crypt_str("Sucessfully mapped DLL!\n");
				Sleep(500);
				bDestroy = true;
				break;
			}
			else 
			{
				End(InitializerName);
			}
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}


void Start(api* App)
{
	std::string FilePath = Dirs.GetCommunicationFolder() + GetRandomWord(17) + ".json";

	json j;
	j["random"] = Dirs.GetRandomFolder();
	j["loaderpid"] = GetCurrentProcessId();
	j["user"] = globals.user_name;
	j["password"] = globals.pass_word;

	std::ofstream outfile(FilePath);
	outfile << j;
	outfile.close();

	auto path = GetRandomWord(20) + crypt_str(".exe");
	CreateFileBytes(App, path, crypt_str("384969"));
	Sleep(2500);


	std::vector<uint8_t> CurrentBytes;
	if (!ReadFileToMemory(path, &CurrentBytes))
	{
		std::cout << crypt_str("[+] Can't read target memory!\n");
		Sleep(1000);
		exit(0);
	}

	SHA1 sha1;
	sha1.add(CurrentBytes.data() + 0, CurrentBytes.size());
	TargetHash = sha1.getHash();

	if (TargetHash.empty())
	{
		std::cout << crypt_str("[+] Can't allocate hash memory!\n");
		Sleep(1000);
		exit(0);
	}

	ShellExecute(NULL, "runas", path.c_str(), 0, 0, SW_SHOWDEFAULT);
	std::thread t(Await, App, path);
	t.detach();

	std::thread V(CheckAutenticity, path);
	V.detach();
}


void menu::render()
{
	if (globals.active)
	{
		if (!doOnce)
		{
			load_styles();

			KeyAuthApp.init();
			if (!KeyAuthApp.data.success)
			{
				MessageBox(NULL, KeyAuthApp.data.message.c_str(), std::string(crypt_str("Warning!")).c_str(), MB_OK);
				exit(0);
			}

			if (KeyAuthApp.checkblack())
			{
				abort();
			}

			doOnce = true;
		}

		ImGui::SetNextWindowSize(ImVec2(child_size.x * 1.05, child_size.y * 1.05));

		ImGui::Begin(iw.window_title, &globals.active, iw.window_flags);
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.034, 0.034, 0.034, 255));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.082, 0.078, 0.078, 255));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.f);
			{
				ImGui::SetCursorPos(ImVec2(7, 7));
				ImGui::BeginChild("##MainPanel", child_size, true);
				{
					//ImGui::GetForegroundDrawList()->AddLine(ImVec2(child_size.x * 0.5, 0), ImVec2(0, 0), ImGui::GetColorU32(ImVec4(0.87, 0.00, 0.00, 255)), 1);

					ImGui::SetCursorPos(ImVec2(child_size.x * 0.96, 5));
					ImGui::TextDisabled("X");
					if (ImGui::IsItemClicked())
					{
						globals.active = false;
					}

					if (show_login)
					{
						child_size = ImVec2{ 300, 276 };

						ImGui::SetCursorPos(ImVec2(118, 20));
						ImGui::TextDisabled(crypt_str("Welcome Back"));

						ImGui::SetCursorPos(ImVec2(97, 35));
						ImGui::Text(crypt_str("Log into your account"));

						ImGui::PushItemWidth(260.f);
						{
							ImGui::SetCursorPos(ImVec2(22, 79));
							ImGui::TextDisabled(crypt_str("Username"));

							ImGui::SetCursorPos(ImVec2(20, 95));
							ImGui::InputText(crypt_str("##Username"), globals.user_name, IM_ARRAYSIZE(globals.user_name));
						}
						ImGui::PopItemWidth();

						ImGui::PushItemWidth(260.f);
						{
							ImGui::SetCursorPos(ImVec2(22, 130));
							ImGui::TextDisabled(crypt_str("Password"));

							//ImGui::SetCursorPos(ImVec2(188, 130));
						   // ImGui::TextDisabled("Forgot password?");

							ImGui::SetCursorPos(ImVec2(20, 146));
							ImGui::InputText(crypt_str("##Passowrd"), globals.pass_word, IM_ARRAYSIZE(globals.pass_word), ImGuiInputTextFlags_Password);
						}
						ImGui::PopItemWidth();

						ImGui::SetCursorPos(ImVec2(22, 190));
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);
						if (ImGui::Button(crypt_str("Login"), ImVec2(260.f, 30.f)))
						{
							KeyAuthApp.login(globals.user_name, globals.pass_word);
							MessageBox(NULL, KeyAuthApp.data.message.c_str(), std::string(crypt_str("Warning!")).c_str(), MB_OK);

							if (KeyAuthApp.data.success)
							{
								show_logged = true;
								show_login = false;
								show_register = false;
							}
						}

						ImGui::PopStyleVar();

						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40, 0.40, 0.40, 255));
						if (ImGui::TextDisabledButton(crypt_str("Don't have an account? Sign up!"), ImVec2(260, 30), ImGuiButtonFlags_None)) {
							show_login = false;
							show_register = true;
						}
						ImGui::PopStyleColor();


					}

					if (show_register)
					{
						child_size = ImVec2{ 300, 310 };


						ImGui::SetCursorPos(ImVec2(118, 20));
						ImGui::TextDisabled(crypt_str("Welcome Back"));

						ImGui::SetCursorPos(ImVec2(95, 35));
						ImGui::Text(crypt_str("Register For An Account"));

						ImGui::PushItemWidth(260.f);
						{
							ImGui::SetCursorPos(ImVec2(22, 79));
							ImGui::TextDisabled(crypt_str("Username"));

							ImGui::SetCursorPos(ImVec2(20, 95));
							ImGui::InputText("##Username", globals.user_name, IM_ARRAYSIZE(globals.user_name));
						}
						ImGui::PopItemWidth();

						ImGui::PushItemWidth(260.f);
						{
							ImGui::SetCursorPos(ImVec2(22, 130));
							ImGui::TextDisabled(crypt_str("Password"));

							ImGui::SetCursorPos(ImVec2(20, 146));
							ImGui::InputText(crypt_str("##Passowrd"), globals.pass_word, IM_ARRAYSIZE(globals.pass_word), ImGuiInputTextFlags_Password);
						}
						ImGui::PopItemWidth();

						ImGui::PushItemWidth(260.f);
						{
							ImGui::SetCursorPos(ImVec2(22, 181));
							ImGui::TextDisabled(crypt_str("Subscription Key"));

							ImGui::SetCursorPos(ImVec2(20, 197));
							ImGui::InputText(crypt_str("##InviteKey"), globals.invite_key, IM_ARRAYSIZE(globals.invite_key), ImGuiInputTextFlags_Password);
						}
						ImGui::PopItemWidth();


						ImGui::SetCursorPos(ImVec2(22, 241));
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);
						if (ImGui::Button(crypt_str("Register"), ImVec2(260.f, 30.f)))
						{
							KeyAuthApp.regstr(globals.user_name, globals.pass_word, globals.invite_key);
							MessageBox(NULL, KeyAuthApp.data.message.c_str(), std::string(crypt_str("Warning!")).c_str(), MB_OK);

							if (KeyAuthApp.data.success) {
								show_logged = true;
								show_register = false;
								show_login = false;
							}
						}
						ImGui::PopStyleVar();

						//  ImGui::SetCursorPos(ImVec2(-5, 237));
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40, 0.40, 0.40, 255));
						if (ImGui::TextDisabledButton(crypt_str("Already have an account? Sign in now!"), ImVec2(260, 30), ImGuiButtonFlags_None)) {
							show_login = true;
							show_register = false;
						}
						ImGui::PopStyleColor();
					}

					if (show_logged)
					{
						child_size = ImVec2{ 300, 276 };
						std::string Status = crypt_str("OFFLINE");
						static bool GotStatus = false;
						static int ReqStatus = 0;
						if (!GotStatus)
						{
							ReqStatus = stoi(KeyAuthApp.var(crypt_str("STATUS")));
							GotStatus = true;
						}

						if (ReqStatus)
							Status = crypt_str("ONLINE");

						ImGui::SetCursorPos(ImVec2(110, 20));
						ImGui::TextDisabled(std::string(crypt_str("Welcome Back ") + std::string(globals.user_name) + crypt_str("!")).c_str());

						ImGui::SetCursorPos(ImVec2(118, 40));
						ImGui::Text(crypt_str("Status: "));
						ImGui::PushStyleColor(ImGuiCol_Text, ReqStatus ? ImVec4(0.38, 1.00, 0.04, 255) : ImVec4(0.87, 0.00, 0.00, 255));
						ImGui::SetCursorPos(ImVec2(158, 40));
						ImGui::Text(Status.c_str());
						ImGui::PopStyleColor();

						ImGui::SetCursorPos(ImVec2(22, 190));
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);
						if (ImGui::Button(crypt_str("Launch!"), ImVec2(260.f, 30.f)))
						{
							if (ReqStatus)
							{
								Start(&KeyAuthApp);
							}
						}
						ImGui::PopStyleVar();
					}


				}
				ImGui::EndChild();
			}
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar(1);

			ImGui::SetCursorPos(ImVec2(5, 445));
			//ImGui::TextDisabled("Loader base, made with <3 by booget#0001");
		}
		ImGui::End();
	}
	else
	{
		exit(0);
	}
}