#pragma once
#include "sdk.h"

// If the sigs return nothing, try removing a few bytes and searching again. sometimes things break.

namespace offsets {

	constexpr uintptr_t refdef = 0x135CD308;
	constexpr uintptr_t name_array = 0x135ECA00;
	constexpr uintptr_t name_array_pos = 0x5E70;
	constexpr uintptr_t loot_ptr = 0xB81D1AC;
	constexpr uintptr_t camera_base = 0x13BEC250;
	constexpr uintptr_t camera_pos = 0x1F8;
	constexpr uintptr_t local_index = 0x1BAB0;
	constexpr uintptr_t local_index_pos = 0x2D0;
	constexpr uintptr_t game_mode = 0xFDDBE08;
	constexpr uintptr_t weapon_definitions = 0x13550A80;
	constexpr uintptr_t distribute = 0xB7E6A50;
	constexpr uintptr_t visible_offset = 0xA80;
	constexpr uintptr_t visible = 0x27C9B50;
	static uintptr_t name_size = 0xD0;
	namespace player {
		constexpr uintptr_t size = 0x6990;
		constexpr uintptr_t valid = 0x5685;
		constexpr uintptr_t pos = 0x6518;
		constexpr uintptr_t team = 0xA7C;
		constexpr uintptr_t weapon_index = 0x6474;
		constexpr uintptr_t dead_1 = 0x6635;
		constexpr uintptr_t dead_2 = 0x55E4;
	}

	namespace bone {
		static uintptr_t base_pos = 0x3CCA8; // Sig: C5 FB 10 86 ? ? ? ? C5 FB 11 85 ? ? ? ? 8B 86 ? ? ? ? 89 85 ? ? ? ?
		static uintptr_t index_struct_size = 0x180;
	}
}