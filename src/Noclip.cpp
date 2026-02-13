///////////////////////////////////////////
//             Noclip
// Removes the player from Yesclip
////////////////////////////////////////////
#include "pch.h"
#include "Noclip.hpp"
#include "Console.hpp"
#include "Hook.hpp"
#include "FuncPointers.h"
#include <array>

uintptr_t Noclip::base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;
bool Noclip::isActive = false;

void Noclip::init() {
	Console::infoPrint("Noclip::init()");
}

//This works for now
void Noclip::toggle() {
	constexpr std::array<unsigned char, 6> DISABLE_NOCLIP_PATCH_BYTES = { 0x0F, 0x87, 0xDB, 0x04, 0x00, 0x00 }; //original
	constexpr std::array<unsigned char, 6> ENABLE_NOCLIP_PATCH_BYTES = { 0xE9, 0x5F, 0x02, 0x00, 0x00, 0x90}; //mod
	HANDLE pHandle = GetCurrentProcess();
	if (Noclip::isActive) {
		Console::print("Noclip: OFF");
		Functions::_SV_SendServerCommand(0i64, 0, "%c \"Noclip: ^1OFF\"", 101i64);
		WriteProcessMemory(pHandle, (LPVOID)(base + 0x39C0A2), DISABLE_NOCLIP_PATCH_BYTES.data(), DISABLE_NOCLIP_PATCH_BYTES.size(), nullptr);
	}
	else {
		Console::print("Noclip: ON");
		Functions::_SV_SendServerCommand(0i64, 0, "%c \"Noclip: ^2ON\"", 101i64);
		WriteProcessMemory(pHandle, (LPVOID)(base + 0x39C0A2), ENABLE_NOCLIP_PATCH_BYTES.data(), ENABLE_NOCLIP_PATCH_BYTES.size(), nullptr);
	}
	Noclip::isActive = !Noclip::isActive;
}