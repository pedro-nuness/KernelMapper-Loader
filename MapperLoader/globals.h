#pragma once

#include "ImGui/ImGui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"


class c_globals {
public:
	std::string name = crypt_str( "DayzCheat" ); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
	std::string ownerid = crypt_str( "OBz74zBgIX" ); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
	std::string secret = crypt_str( "432f0279997bf737f1581324515a5b923d0bcb73d95ca5f47f0399248b037967" ); // app secret, the blurred text on licenses tab and other tabs
	std::string version = crypt_str( "1.0" ); // leave alone unless you've changed version on website
	std::string url = crypt_str( "https://keyauth.win/api/1.2/" ); // change if you're self-hosting


	std::string TargetID = "";
	std::string BuildID = "";
	std::string DriverID = "";


	std::string ExePath = "";
	std::string DriverPath = "";
	std::string JsonPath = "";
	std::string TargetHash = "";

	std::string AuthenticHash;

	std::string ResponseID = "";

	std::string Last_Update;

	int TargetPID;
	int Status;

	TempFiles Directories = SetupDirectory( );


	bool active = true;

	char user_name[ 255 ] = "";
	char pass_word[ 255 ] = "";

	char invite_key[ 255 ] = "";

	int button_opacity = 255;
	bool bDoneWindow = false;

	std::vector<std::string>
		List {
			crypt_str( "ida64.exe" ),
			crypt_str( "ida32.exe" ),
			crypt_str( "Wireshark.exe" ),
			crypt_str( "ProcessHacker.exe" ),
			crypt_str( "HTTPDebuggerUI" ),
			crypt_str( "Charles.exe" ),
			crypt_str( "x64dbg.exe" ),
			crypt_str( "x32dbg.exe" ),
			crypt_str( "DnSpy.exe" ),
	};

} globals;

class initWindow
{
public:
	const char * window_title = "Loader";
	ImVec2 window_size { 740, 460 };

	DWORD window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse;
} iw;

void load_styles( )
{
	ImVec4 * colors = ImGui::GetStyle( ).Colors;
	{
		colors[ ImGuiCol_WindowBg ] = ImVec4( 0.06f , 0.06f , 0.06f , 1.00f );

		colors[ ImGuiCol_FrameBg ] = ImColor( 11 , 11 , 11 , 255 );
		colors[ ImGuiCol_FrameBgHovered ] = ImColor( 11 , 11 , 11 , 255 );

		colors[ ImGuiCol_Button ] = ImColor( 255 , 0 , 46 , globals.button_opacity );
		colors[ ImGuiCol_ButtonActive ] = ImColor( 255 , 0 , 46 , globals.button_opacity );
		colors[ ImGuiCol_ButtonHovered ] = ImColor( 255 , 0 , 46 , globals.button_opacity );

		colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.37f , 0.37f , 0.37f , 1.00f );
	}

	ImGuiStyle * style = &ImGui::GetStyle( );
	{
		style->WindowPadding = ImVec2( 4 , 4 );
		style->WindowBorderSize = 0.f;

		style->FramePadding = ImVec2( 8 , 6 );
		style->FrameRounding = 3.f;
		style->FrameBorderSize = 1.f;
	}
}

bool CheckStrings( std::string bString1 , std::string bExpectedResult )
{
	size_t found = bString1.find( bExpectedResult );
	if ( found != std::string::npos )
	{
		return true;
	}

	return false;
}


bool KillProcess( int PID)
{
	HANDLE hProcess = OpenProcess( PROCESS_TERMINATE , FALSE , PID );

	if ( hProcess == NULL ) {
		return false;
	}
	else
	{
		// Fechar o processo
		TerminateProcess( hProcess , 0 );

		// Liberar o handle do processo
		CloseHandle( hProcess );
	}
	return true;
}



DWORD GetProcessPID( LPCTSTR ProcessName ) // non-conflicting function name
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS , 0 );
	pt.dwSize = sizeof( PROCESSENTRY32 );
	if ( Process32First( hsnap , &pt ) ) { // must call this first
		do {
			if ( !lstrcmpi( pt.szExeFile , ProcessName ) ) {
				CloseHandle( hsnap );
				return pt.th32ProcessID;
			}
		} while ( Process32Next( hsnap , &pt ) );
	}
	CloseHandle( hsnap ); // close handle on failure
	return 0;
}


void Log( std::string message , bool breakline = true )
{
	if ( breakline )
		std::cout << message << std::endl;
	else
		std::cout << message;
}

void Error( std::string message ) {

	auto ExePID = GetProcessPID( globals.ExePath.c_str( ));

	if ( ExePID )
		KillProcess( ExePID );

	Sleep( 500 );

	remove( globals.ExePath.c_str( ) );

	::ShowWindow( ::GetConsoleWindow( ) , SW_SHOW ); //Show console

	Log( message );

	Sleep( 4000 );
	exit( 0 );
}

bool ReadFileToMemory( const std::string & file_path , std::vector<uint8_t> * out_buffer )
{
	std::ifstream file_ifstream( file_path , std::ios::binary );

	if ( !file_ifstream )
		return false;

	//out_buffer->assign((std::istreambuf_iterator<char>(file_ifstream)), std::istreambuf_iterator<char>());
	file_ifstream.close( );

	return true;
}

std::string GetFileHash( std::string & path )
{
	std::vector<uint8_t> CurrentBytes;
	if ( !ReadFileToMemory( path , &CurrentBytes ) )
	{
		Log( crypt_str( "[+] Can't read target memory!" ) );
		Sleep( 1000 );
		exit( 0 );
	}

	SHA1 sha1;
	sha1.add( CurrentBytes.data( ) + 0 , CurrentBytes.size( ) );
	return sha1.getHash( );
}

void CreateFileBytes( api * app , std::string path , std::string FileID )
{
	std::vector<std::uint8_t> bytes = KeyAuthApp.download( FileID );

	if ( !KeyAuthApp.data.success ) // check whether file downloaded correctly
	{
		MessageBox( NULL , KeyAuthApp.data.message.c_str( ) , std::string( crypt_str( "Warning!" ) ).c_str( ) , MB_OK );
		exit( 0 );
	}

	std::ofstream file( path , std::ios_base::out | std::ios_base::binary );
	file.write( ( char * ) bytes.data( ) , bytes.size( ) );
	file.close( );
}


std::string GetThisAppName( ) {
	char filename[ MAX_PATH ];
	DWORD size = GetModuleFileNameA( NULL , filename , MAX_PATH );
	if ( !size ) {
		Log( crypt_str( "Could not fine EXE file name!" ) );
		return "";
	}

	//Remove everything before the last "\"
	std::string name = filename;
	auto it = std::find( name.rbegin( ) , name.rend( ) , '\\' ); //escape the escape character
	if ( it != name.rend( ) ) {
		name.erase( name.begin( ) , it.base( ) );
	}

	return filename;
}

#include <mutex>

namespace Driver
{

	std::once_flag flag;

	template<typename ... A>
	uint64_t CallHook( const A ... arguments )
	{
		std::call_once( flag , [ ] { LoadLibrary( "user32.dll" ); } );
		void * control_function = nullptr;
		control_function = GetProcAddress( LoadLibrary( "win32u.dll" ) , "NtOpenCompositionSurfaceSectionInfo" );
		if ( control_function != nullptr ) {
			const auto control = static_cast< uint64_t( __stdcall * )( A... ) >( control_function );
			return control( arguments ... );
		}

		return 0;
	}

	AnswareManager CheckDriver( )
	{
		AnswareManager DefaultAnswer = AnswareManager( true );

		_COPY_MEMORY m {};
		m.kFunction = COMMUNICATION;
		m.kPid = NULL;
		m.kAddress = NULL;
		m.kBuffer = nullptr;
		m.kSize = NULL;
		m.kModuleName = NULL;
		m.KProcessName = NULL;
		m.kAnswer = &DefaultAnswer;

		CallHook( &m );

		free( DefaultAnswer.kResponse );

		return DefaultAnswer;
	}

}