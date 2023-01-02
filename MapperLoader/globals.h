#pragma once

#include "ImGui/ImGui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

class c_globals {
public:
	bool active = true;
	
	char user_name[255] = "";
	char pass_word[255] = "";

	char invite_key[255] = "";

	int button_opacity = 255;
	bool bDoneWindow = false;

} globals;