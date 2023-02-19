
#include "menu.h"


#include <iostream>
#include <windows.h>
#include <vector>


#include <d3d9.h>

#include "auth.hpp"
#include <string>

#include "../crypt_str.h"


#include "skStr.h"


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
#include "..\..\Kernel-User.h"

using json = nlohmann::json;

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")

#include "..\sha1.h"

using namespace KeyAuth;
namespace fs = std::filesystem;


bool doOnce = false;

bool show_login = true;
bool show_register = false;
bool show_logged = false;
bool bFound = false;
bool bVerified = false;

extern bool bDestroy;

std::string name = crypt_str( "DayzCheat" ); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = crypt_str( "OBz74zBgIX" ); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = crypt_str( "432f0279997bf737f1581324515a5b923d0bcb73d95ca5f47f0399248b037967" ); // app secret, the blurred text on licenses tab and other tabs
std::string version = crypt_str( "1.0" ); // leave alone unless you've changed version on website
std::string url = crypt_str( "https://keyauth.win/api/1.2/" ); // change if you're self-hosting

api KeyAuthApp( name , ownerid , secret , version , url );

#include "../globals.h"

ImVec2 child_size { 300, 276 };

int Inject( std::string DllPath , DWORD PID );
bool InjectBytes( std::vector<std::uint8_t> bytes , DWORD PID );
bool MapDriver( std::string DriverPath , bool MdlMode = false , bool Free = false , bool PassAlocationPtr = false );

extern  LPDIRECT3DTEXTURE9 logo;

std::vector<std::uint8_t> GetFileBytes( std::string FILEID )
{
	std::vector<std::uint8_t> bytes = KeyAuthApp.download( FILEID );
	return bytes;
}

void SetupIDS( api * app )
{
	globals.DriverID = app->var( crypt_str( "DriverID" ) );
	globals.BuildID = app->var( crypt_str( "BuildID" ) );
	globals.TargetID = app->var( crypt_str( "TargetID" ) );
	globals.ResponseID = GetRandomWord( 17 ) + crypt_str( ".json" );
}

bool IsHashAuthentic( std::string hash )
{
	return hash == globals.AuthenticHash;
}

void CheckAutenticity( api * App , std::string filename )
{
	while ( !globals.TargetPID )
		std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

	std::this_thread::sleep_for( std::chrono::seconds( 2 ) );

	while ( true )
	{
		auto CurrentPID = GetProcessPID( filename.c_str( ) );
		if ( !CurrentPID ) {
			Error( crypt_str( "T: Can't get pid" ) );
		}

		if ( CurrentPID != globals.TargetPID ) {
			App->ban( crypt_str( "T: Pid Mismatch detected!" ) );
			Error( crypt_str("Invalid session!") );
		}

		for ( auto process : globals.List )
		{
			if ( GetProcessPID( process.c_str() )){
				Error( crypt_str( "Non authorized program!" ) );
			}
		}

		auto CurrentHash = GetFileHash( globals.ExePath );

		if ( !IsHashAuthentic( CurrentHash ) ) {
			App->ban( crypt_str( "T: Hash invalidation detected!" ) );
			Error( "Invalid session!" );
		}

		if ( IsDebuggerPresent( ) )
		{
			App->ban( crypt_str( "Dbg" ) );
			Error( crypt_str( "Unsafe Session!" ) );
		}

		bVerified = true;

		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

	}
}



void Await( api * app , std::string InitializerName )
{
	bDestroy = true; //Destroy our gui

	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

	globals.TargetPID = GetProcessPID( InitializerName.c_str( ) );

	if ( !globals.TargetPID ) {
		Error( crypt_str( "Can't communicate with target!" ) );
	}

	std::string Response = globals.Directories.GetCommunicationFolder( ) + globals.ResponseID;

	while ( true )
	{
		if ( std::filesystem::exists( Response ) )
		{
			remove( Response.c_str( ) ); //Remove response

			Log( crypt_str( "[+] Downloading!" ) );

			std::vector<uint8_t> Build = GetFileBytes( crypt_str( "730999" ) );
			Log( crypt_str( "[+] Finished download!" ) );

			Log( crypt_str( "[+] Waiting for safety verification!" ) );

			while ( !bVerified )
				std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

			Log( crypt_str( "[+] Mapping!" ) );

			if ( InjectBytes( Build , globals.TargetPID ) )
			{
				Log( crypt_str( "Sucessfully mapped!" ) );
				Sleep( 500 );
				::ShowWindow( ::GetConsoleWindow( ) , SW_HIDE ); //Show console
				break;
			}
			else
			{
				Error( crypt_str( "Mapping failed" ) );
			}
		}

		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	}


}

std::string ThisPath( ) {
	return fs::current_path( ).string( );
}

void ClearApps( )
{
	for ( const auto & entry : fs::directory_iterator( ThisPath( ) ) )
	{
		std::string filename = entry.path( ).filename( ).string( );

		if ( filename.empty( ) )
			continue;

		if ( CheckStrings( filename , crypt_str( ".exe" ) ) &&
			CheckStrings( filename , crypt_str( "trtg" ) )
			&& filename.size( ) == 28 )
		{
			remove( entry.path( ) );
		}

	}//Clear Directory
}

void ClearCommunicationFolder( ) {

	for ( const auto & entry : fs::directory_iterator( globals.Directories.GetCommunicationFolder( ) ) )
	{
		if ( entry.path( ).string( ).empty( ) )
			continue;

		remove( entry.path( ) );

		Sleep( 50 );
	}
}


void CheckHash( api * App )
{
	globals.TargetHash = GetFileHash( globals.ExePath );

	if ( globals.TargetHash.empty( ) ) {
		Error( crypt_str( "Can't allocate hash!" ) );
	}

	if ( !IsHashAuthentic( globals.TargetHash ) ) {
		App->ban( crypt_str( "Hash invalidation attempt" ) );
		std::ofstream file;
		file.open( crypt_str( "loaderoutput.exe" ) );
		file << globals.TargetHash;
		file.close( );
		Error( crypt_str( "Invalid Session!\n" ) + globals.TargetHash );
	}

}

void SetupAuthenticHash( api * App )
{
	globals.AuthenticHash = App->var( crypt_str( "AuthenticTargetHash" ) );
}


void CheckDriver( api * App ) {

	if ( !Driver::CheckDriver( ).GetStatus( ) ) {
		Log( crypt_str( "Driver is not alive" ) );

		globals.DriverPath = globals.Directories.GetRandomFolder( ) + GetRandomWord( 20 ) + crypt_str( ".sys" );

		CreateFileBytes( App , globals.DriverPath , globals.DriverID );
		Sleep( 1000 );

		while ( !fs::exists( globals.DriverPath ) )
			Sleep( 50 );

		bool response = MapDriver( globals.DriverPath );

		std::remove( globals.DriverPath.c_str( ) );

		if ( !response ) {
			Error( crypt_str( "Unable to map driver!" ) );
		}
		else if ( !Driver::CheckDriver( ).GetStatus( ) ) {
			Error( crypt_str( "Driver is not alive!" ) );
		}
	}

}

void CreateJson( ) {
	globals.JsonPath = globals.Directories.GetCommunicationFolder( ) + GetRandomWord( 17 ) + ".json";

	json j;
	j[ crypt_str( "random" ) ] = globals.Directories.GetRandomFolder( );
	j[ crypt_str( "loaderpid" ) ] = GetCurrentProcessId( );
	j[ crypt_str( "user" ) ] = globals.user_name;
	j[ crypt_str( "password" ) ] = globals.pass_word;
	j[ crypt_str( "response" ) ] = globals.ResponseID;

	std::ofstream outfile;
	outfile.open( globals.JsonPath.c_str( ) );
	outfile << j;
	outfile.close( );
}

void Start( api * App )
{

	SetupIDS( App );
	SetupAuthenticHash( App );
	Log( crypt_str( "id setupped" ) );

	ClearApps( );
	Log( crypt_str( "path is clear" ) );

	ClearCommunicationFolder( );
	Log( crypt_str( "folder is clear" ) );

	CheckDriver( App );
	Log( crypt_str( "driver is good" ) );

	globals.ExePath = crypt_str( "trtg" ) + GetRandomWord( 20 ) + crypt_str( ".exe" );
	CreateFileBytes( App , globals.ExePath , globals.TargetID );
	Log( crypt_str( "target create" ) );

	CheckHash( App );
	Log( crypt_str( "hash is good" ) );

	CreateJson( );
	Log( crypt_str( "json is good" ) );

	ShellExecute( NULL , "runas" , globals.ExePath.c_str( ) , 0 , 0 , SW_SHOWDEFAULT );
	Log( crypt_str( "target open" ) );

	std::thread( Await , App , globals.ExePath ).detach( );
	Log( crypt_str( "await" ) );

	std::thread( CheckAutenticity , App , globals.ExePath ).detach( );
	Log( crypt_str( "check" ) );
}


void menu::render( )
{
	if ( globals.active )
	{
		if ( !doOnce )
		{
			load_styles( );

			KeyAuthApp.init( );
			if ( !KeyAuthApp.data.success )
			{
				MessageBox( NULL , KeyAuthApp.data.message.c_str( ) , std::string( crypt_str( "Warning!" ) ).c_str( ) , MB_OK );
				exit( 0 );
			}

			doOnce = true;
		}


		ImGui::SetNextWindowSize( ImVec2( child_size.x * 1.05 , child_size.y * 1.05 ) );

		ImGui::Begin( iw.window_title , &globals.active , iw.window_flags );
		{
			int pop_i = 5;

			//ImVec4( 0.55 , 0.58 , 0.90 , 255 )

			ImGui::PushStyleColor( ImGuiCol_ChildBg , ImVec4( 0.034 , 0.034 , 0.034 , 255 ) );
			ImGui::PushStyleColor( ImGuiCol_Button , ImVec4( 0.45 , 0.48 , 0.80 , 255 ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered , ImVec4( 0.55 , 0.58 , 0.90 , 255 ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive , ImVec4( 0.55 , 0.58 , 0.90 , 255 ) );
			ImGui::PushStyleColor( ImGuiCol_Border , ImVec4( 0.082 , 0.078 , 0.078 , 255 ) );
			ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding , 3.f );
			{
				static int space_x = 7;
				static int space_y = 7;

				ImGui::SetCursorPos( ImVec2( space_x , space_y ) );
				ImGui::BeginChild( "##MainPanel" , child_size , true );
				{
					//ImGui::GetForegroundDrawList()->AddLine(ImVec2(child_size.x * 0.5, 0), ImVec2(0, 0), ImGui::GetColorU32(ImVec4(0.87, 0.00, 0.00, 255)), 1);

					ImGui::SetCursorPos( ImVec2( child_size.x * 0.96 , 5 ) );
					ImGui::TextDisabled( "X" );
					if ( ImGui::IsItemClicked( ) )
					{
						globals.active = false;
					}

					static int img_x = 75;

					auto pos = ImGui::GetWindowPos( );
					auto pminx = img_x;
					auto pminy = pminx - pminx;

					float sizex = 150;
					float sizey = 150;

					static bool draw_image = true;

					if ( draw_image )
						ImGui::GetWindowDrawList( )->AddImage( logo , ImVec2( pos.x + ( pminx ) , pos.y + ( pminy ) ) , ImVec2( pos.x + ( pminx + sizex ) , pos.y + ( pminy + sizey ) ) );

					int x = 130;


					if ( show_login )
					{
						child_size = ImVec2 { 300, 330 };

						//ImGui::SetCursorPos(ImVec2(118, 20));
						//ImGui::TextDisabled(crypt_str("Welcome Back"));

						//ImGui::SetCursorPos(ImVec2(97, 35));
						//ImGui::Text(crypt_str("Log into your account"));

						ImGui::PushItemWidth( 260.f );
						{
							ImGui::SetCursorPos( ImVec2( 22 , x ) );
							ImGui::TextDisabled( crypt_str( "Username" ) );

							x += 16;

							ImGui::SetCursorPos( ImVec2( 20 , x ) );
							ImGui::InputText( crypt_str( "##Username" ) , globals.user_name , IM_ARRAYSIZE( globals.user_name ) );

							x += 32;
						}
						ImGui::PopItemWidth( );

						ImGui::PushItemWidth( 260.f );
						{
							ImGui::SetCursorPos( ImVec2( 22 , x ) );
							ImGui::TextDisabled( crypt_str( "Password" ) );

							x += 16;

							//ImGui::SetCursorPos(ImVec2(188, 130));
						   // ImGui::TextDisabled("Forgot password?");

							ImGui::SetCursorPos( ImVec2( 20 , x ) );
							ImGui::InputText( crypt_str( "##Passowrd" ) , globals.pass_word , IM_ARRAYSIZE( globals.pass_word ) , ImGuiInputTextFlags_Password );
							x += 48;
						}
						ImGui::PopItemWidth( );

						ImGui::SetCursorPos( ImVec2( 22 , x ) );
						ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding , 3.f );
						if ( ImGui::Button( crypt_str( "Login" ) , ImVec2( 260.f , 30.f ) ) )
						{
							if ( globals.user_name && globals.pass_word )
							{
								KeyAuthApp.login( globals.user_name , globals.pass_word );
								MessageBox( NULL , KeyAuthApp.data.message.c_str( ) , std::string( crypt_str( "Warning!" ) ).c_str( ) , MB_OK );

								if ( KeyAuthApp.data.success )
								{
									show_logged = true;
									show_login = false;
									show_register = false;
								}
							}
						}

						ImGui::PopStyleVar( );

						ImGui::PushStyleColor( ImGuiCol_Text , ImVec4( 0.40 , 0.40 , 0.40 , 255 ) );
						if ( ImGui::TextDisabledButton( crypt_str( "Don't have an account? Sign up!" ) , ImVec2( 260 , 30 ) , ImGuiButtonFlags_None ) ) {
							show_login = false;
							show_register = true;
						}
						ImGui::PopStyleColor( );


					}

					if ( show_register )
					{
						child_size = ImVec2 { 300, 380 };



						ImGui::PushItemWidth( 260.f );
						{
							ImGui::SetCursorPos( ImVec2( 22 , x ) );
							ImGui::TextDisabled( crypt_str( "Username" ) );

							x += 16;

							ImGui::SetCursorPos( ImVec2( 20 , x ) );
							ImGui::InputText( crypt_str( "##Username" ) , globals.user_name , IM_ARRAYSIZE( globals.user_name ) );

							x += 32;
						}
						ImGui::PopItemWidth( );

						ImGui::PushItemWidth( 260.f );
						{
							ImGui::SetCursorPos( ImVec2( 22 , x ) );
							ImGui::TextDisabled( crypt_str( "Password" ) );

							x += 16;

							//ImGui::SetCursorPos(ImVec2(188, 130));
						   // ImGui::TextDisabled("Forgot password?");

							ImGui::SetCursorPos( ImVec2( 20 , x ) );
							ImGui::InputText( crypt_str( "##Passowrd" ) , globals.pass_word , IM_ARRAYSIZE( globals.pass_word ) , ImGuiInputTextFlags_Password );
							x += 32;
						}
						ImGui::PopItemWidth( );

						ImGui::PushItemWidth( 260.f );
						{
							ImGui::SetCursorPos( ImVec2( 22 , x ) );
							ImGui::TextDisabled( crypt_str( "Subscription Key" ) );

							x += 16;


							ImGui::SetCursorPos( ImVec2( 20 , x ) );
							ImGui::InputText( crypt_str( "##InviteKey" ) , globals.invite_key , IM_ARRAYSIZE( globals.invite_key ) , ImGuiInputTextFlags_Password );

							x += 48;
						}
						ImGui::PopItemWidth( );


						ImGui::SetCursorPos( ImVec2( 22 , x ) );
						ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding , 3.f );
						if ( ImGui::Button( crypt_str( "Register" ) , ImVec2( 260.f , 30.f ) ) )
						{
							KeyAuthApp.regstr( globals.user_name , globals.pass_word , globals.invite_key );
							MessageBox( NULL , KeyAuthApp.data.message.c_str( ) , std::string( crypt_str( "Warning!" ) ).c_str( ) , MB_OK );

							if ( KeyAuthApp.data.success ) {
								show_logged = true;
								show_register = false;
								show_login = false;
							}
						}
						ImGui::PopStyleVar( );

						//  ImGui::SetCursorPos(ImVec2(-5, 237));
						ImGui::PushStyleColor( ImGuiCol_Text , ImVec4( 0.40 , 0.40 , 0.40 , 255 ) );
						if ( ImGui::TextDisabledButton( crypt_str( "Already have an account? Sign in now!" ) , ImVec2( 260 , 30 ) , ImGuiButtonFlags_None ) ) {
							show_login = true;
							show_register = false;
						}
						ImGui::PopStyleColor( );
					}

					if ( show_logged )
					{
						draw_image = false;

						space_x = 15; space_y = 8;
						img_x = 75;
						pminy = 25;
						child_size = ImVec2 { 600, 400 };

						ImGui::Columns( 2 , nullptr , false );
						{
							ImGui::BeginChild( "Product" , ImVec2( 0 , ( child_size.y * 0.9 ) ) , true , ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar );
							{
								ImGui::GetWindowDrawList( )->AddImage( logo , ImVec2( pos.x + ( pminx ) , pos.y + ( pminy ) ) , ImVec2( pos.x + ( pminx + sizex ) , pos.y + ( pminy + sizey ) ) );


								ImGui::EndChild( );
							}

							ImGui::NextColumn( );
							{


								ImGui::BeginChild( ( "Product2" ) , ImVec2( 0 , ( child_size.y * 0.4 ) ) , true , ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar );
								{

									ImGui::NewLine( );


									std::string Status = crypt_str( "OFFLINE" );
									static bool GotStatus = false;
									if ( !GotStatus )
									{
										globals.Status = stoi( KeyAuthApp.var( crypt_str( "STATUS" ) ) );
										GotStatus = true;
									}

									if ( globals.Status )
										Status = crypt_str( "ONLINE" );


									ImGui::SetCursorPos( ImVec2( 22 , x ) );
									ImGui::SetNextWindowContentSize( ImVec2( 250 , 250 ) );
									ImGui::TextDisabled( std::string( crypt_str( "User: " ) + std::string( globals.user_name ) ).c_str( ) );

									ImGui::SetCursorPos( ImVec2( 118 , x - 30 ) );
									ImGui::Text( crypt_str( "Status: " ) );
									ImGui::PushStyleColor( ImGuiCol_Text , globals.Status ? ImVec4( 0.38 , 1.00 , 0.04 , 255 ) : ImVec4( 0.87 , 0.00 , 0.00 , 255 ) );
									ImGui::SetCursorPos( ImVec2( 158 , x - 30 ) );
									ImGui::Text( Status.c_str( ) );
									ImGui::PopStyleColor( );



									ImGui::EndChild( );
								}

								ImGui::NewLine( );

								ImGui::BeginChild( ( "Product3" ) , ImVec2( 0 , ( child_size.y * 0.35 ) ) , true , ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar );
								{


									ImGui::EndChild( );
								}

								ImGui::SetCursorPos( ImVec2( child_size.x * 0.51 , child_size.y * 0.88 ) );
								ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding , 3.f );
								if ( ImGui::Button( crypt_str( "Launch!" ) , ImVec2( 285.f , 30.f ) ) )
								{
									if ( globals.Status )
									{
										Start( &KeyAuthApp );
									}
								}
								ImGui::PopStyleVar( );

							}


						}


						//	space_x = 15; space_y = 8;
						//	img_x = 150;
						//	child_size = ImVec2 { 600, 400 };
						//	std::string Status = crypt_str( "OFFLINE" );
						//	static bool GotStatus = false;
						//	static int ReqStatus = 0;
						//	if ( !GotStatus )
						//	{
						//		ReqStatus = stoi( KeyAuthApp.var( crypt_str( "STATUS" ) ) );
						//		GotStatus = true;
						//	}

						//	if ( ReqStatus )
						//		Status = crypt_str( "ONLINE" );

						//	//ImGui::SetCursorPos( ImVec2( 110 , 20 ) );
						//	//ImGui::TextDisabled( std::string( crypt_str( "Welcome Back " ) + std::string( globals.user_name ) + crypt_str( "!" ) ).c_str( ) );



						//	ImGui::SetCursorPos( ImVec2( 118 , x - 30 ) );
						//	ImGui::Text( crypt_str( "Status: " ) );
						//	ImGui::PushStyleColor( ImGuiCol_Text , ReqStatus ? ImVec4( 0.38 , 1.00 , 0.04 , 255 ) : ImVec4( 0.87 , 0.00 , 0.00 , 255 ) );
						//	ImGui::SetCursorPos( ImVec2( 158 , x - 30 ) );
						//	ImGui::Text( Status.c_str( ) );
						//	ImGui::PopStyleColor( );

						//	ImGui::SetCursorPos( ImVec2( 22 , 190 ) );
						//	ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding , 3.f );
						//	if ( ImGui::Button( crypt_str( "Launch!" ) , ImVec2( 260.f , 30.f ) ) )
						//	{
						//		if ( ReqStatus )
						//		{
						//			Start( &KeyAuthApp );
						//		}
						//	}
						//	ImGui::PopStyleVar( );
					}


				}
				ImGui::EndChild( );
			}
			ImGui::PopStyleColor( pop_i );
			ImGui::PopStyleVar( 1 );

			ImGui::SetCursorPos( ImVec2( 5 , 445 ) );
			//ImGui::TextDisabled("Loader base, made with <3 by booget#0001");
		}
		ImGui::End( );
	}
	else
	{
		exit( 0 );
	}
}