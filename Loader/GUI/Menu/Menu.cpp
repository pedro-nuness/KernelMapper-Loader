#include "Menu.h"
#include "../../Globals/globals.h"
#include "../../GUI/ImGui/imgui.h"

#include <d3d9.h>
#include <thread>
#include <Auth/auth.hpp>

#include "../../Launch/Launch.h"

bool doOnce = false;

bool show_login = true;
bool show_register = false;
bool show_logged = false;

ImVec2 child_size { 300, 276 };
using namespace KeyAuth;


extern LPDIRECT3DTEXTURE9 logo;

api KeyAuthApp( Globals::Get( ).name , Globals::Get().ownerid , Globals::Get().secret , Globals::Get( ).version , Globals::Get( ).url );

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

		colors[ ImGuiCol_Button ] = ImColor( 255 , 0 , 46 , Globals::Get().button_opacity );
		colors[ ImGuiCol_ButtonActive ] = ImColor( 255 , 0 , 46 , Globals::Get( ).button_opacity );
		colors[ ImGuiCol_ButtonHovered ] = ImColor( 255 , 0 , 46 , Globals::Get( ).button_opacity );

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



void Menu::Render( ) {

	if ( Globals::Get().Active )
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

			
			Launch::Get( ).ClearApps( );
			std::thread( &Launch::Idle , &Launch::Get( ) ).detach( );

			doOnce = true;
		}


		ImGui::SetNextWindowSize( ImVec2( child_size.x * 1.05 , child_size.y * 1.05 ) );

		ImGui::Begin( iw.window_title , &Globals::Get().Active , iw.window_flags );
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
						Globals::Get( ).Active = false;
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
							ImGui::InputText( crypt_str( "##Username" ) , Globals::Get( ).user_name , IM_ARRAYSIZE( Globals::Get( ).user_name ) );

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
							ImGui::InputText( crypt_str( "##Passowrd" ) , Globals::Get( ).pass_word , IM_ARRAYSIZE( Globals::Get( ).pass_word ) , ImGuiInputTextFlags_Password );
							x += 48;
						}
						ImGui::PopItemWidth( );

						ImGui::SetCursorPos( ImVec2( 22 , x ) );
						ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding , 3.f );
						if ( ImGui::Button( crypt_str( "Login" ) , ImVec2( 260.f , 30.f ) ) )
						{
							if ( Globals::Get( ).user_name && Globals::Get( ).pass_word )
							{
								KeyAuthApp.login( Globals::Get( ).user_name , Globals::Get( ).pass_word );							
						
								if ( KeyAuthApp.data.success )
								{
									Globals::Get( ).Logged = true;
									show_logged = true;
									show_login = false;
									show_register = false;
								}else
									MessageBox( NULL , KeyAuthApp.data.message.c_str( ) , std::string( crypt_str( "Warning!" ) ).c_str( ) , MB_OK );
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
							ImGui::InputText( crypt_str( "##Username" ) , Globals::Get( ).user_name , IM_ARRAYSIZE( Globals::Get( ).user_name ) );

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
							ImGui::InputText( crypt_str( "##Passowrd" ) , Globals::Get( ).pass_word , IM_ARRAYSIZE( Globals::Get( ).pass_word ) , ImGuiInputTextFlags_Password );
							x += 32;
						}
						ImGui::PopItemWidth( );

						ImGui::PushItemWidth( 260.f );
						{
							ImGui::SetCursorPos( ImVec2( 22 , x ) );
							ImGui::TextDisabled( crypt_str( "Subscription Key" ) );

							x += 16;


							ImGui::SetCursorPos( ImVec2( 20 , x ) );
							ImGui::InputText( crypt_str( "##InviteKey" ) , Globals::Get( ).invite_key , IM_ARRAYSIZE( Globals::Get( ).invite_key ) , ImGuiInputTextFlags_Password );

							x += 48;
						}
						ImGui::PopItemWidth( );


						ImGui::SetCursorPos( ImVec2( 22 , x ) );
						ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding , 3.f );
						if ( ImGui::Button( crypt_str( "Register" ) , ImVec2( 260.f , 30.f ) ) )
						{
							KeyAuthApp.regstr( Globals::Get( ).user_name , Globals::Get( ).pass_word , Globals::Get( ).invite_key );
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
										Globals::Get( ).Status = true;
										GotStatus = true;
									}

									if ( Globals::Get( ).Status )
										Status = crypt_str( "ONLINE" );


									ImGui::SetCursorPos( ImVec2( 22 , x ) );
									ImGui::SetNextWindowContentSize( ImVec2( 250 , 250 ) );
									ImGui::TextDisabled( std::string( crypt_str( "User: " ) + std::string( Globals::Get( ).user_name ) ).c_str( ) );

									ImGui::SetCursorPos( ImVec2( 118 , x - 30 ) );
									ImGui::Text( crypt_str( "Status: " ) );
									ImGui::PushStyleColor( ImGuiCol_Text , Globals::Get().Status ? ImVec4( 0.38 , 1.00 , 0.04 , 255 ) : ImVec4( 0.87 , 0.00 , 0.00 , 255 ) );
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
									if ( Globals::Get( ).Status && !Globals::Get().RunningLaunchThread ){
										std::thread( &Launch::Init , &Launch::Get(), &KeyAuthApp ).detach( );
										Globals::Get( ).RunningLaunchThread = true;
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