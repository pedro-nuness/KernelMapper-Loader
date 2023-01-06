#pragma once

#define OFF_LOCAL 0x18AC00
#define OFF_HEALTH 0xEC

namespace Game
{
	ULONG64 Base;
	DriverCommunication *K = nullptr;
	

	uint64_t GetBaseMem(bool adress = false) {
		return K->ReadMemory<uint64_t>(Game::Base);
	}

	uint32_t GetLocalPlayer(bool adress = false)
	{
		return K->ReadMemory<uint32_t>(Game::Base + 0x18AC00);
	}

	uint32_t GetHealth(bool adress = false) {
		return GetLocalPlayer() + 0xEC;
	}

}