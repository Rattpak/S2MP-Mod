//////////////////////////////////////
//       Custom Commands
//	Logic for custom commands
//////////////////////////////////////
#include "pch.h"
#include "CustomCommands.hpp"
#include "Console.hpp"
#include "FuncPointers.h"
#include <array>

uintptr_t CustomCommands::base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;
uintptr_t CustomCommands::rawBase = (uintptr_t)GetModuleHandle(NULL);
bool CustomCommands::isGodmode = false;

void CustomCommands::toggleGodmode() {
	int* health = (int*)(rawBase + 0xA0C740C);
	if (CustomCommands::isGodmode) {
		*health = 100;
		Console::print("God: OFF");
	}
	else {
		*health = -1;
		Console::print("God: ON");
	}
	CustomCommands::isGodmode = !CustomCommands::isGodmode;
}

//cg_drawlui
void CustomCommands::toggleHud(bool b) {
	constexpr std::array<unsigned char, 5> DISABLE_HUD_PATCH_BYTES = { 0x90, 0x90, 0x90, 0x90, 0x90 }; //patch
	constexpr std::array<unsigned char, 5> ENABLE_HUD_PATCH_BYTES = { 0xE8, 0x66, 0xE8, 0xFF, 0xFF }; //original
	HANDLE pHandle = GetCurrentProcess();
	if (b) {
		WriteProcessMemory(pHandle, (LPVOID)(base + 0x960205), ENABLE_HUD_PATCH_BYTES.data(), ENABLE_HUD_PATCH_BYTES.size(), nullptr);
	}
	else {
		WriteProcessMemory(pHandle, (LPVOID)(base + 0x960205), DISABLE_HUD_PATCH_BYTES.data(), DISABLE_HUD_PATCH_BYTES.size(), nullptr);
	}
}

//cg_drawGun
void CustomCommands::toggleGun(bool b) {
	constexpr std::array<unsigned char, 5> DISABLE_GUNDRAW_PATCH_BYTES = { 0x90, 0x90, 0x90, 0x90, 0x90 }; //patch
	constexpr std::array<unsigned char, 5> ENABLE_GUNDRAW_PATCH_BYTES = { 0xE8, 0x97, 0x15, 0x0D, 0x00 }; //original
	HANDLE pHandle = GetCurrentProcess();
	if (b) {
		WriteProcessMemory(pHandle, (LPVOID)(base + 0x7A774), ENABLE_GUNDRAW_PATCH_BYTES.data(), ENABLE_GUNDRAW_PATCH_BYTES.size(), nullptr);
	}
	else {
		WriteProcessMemory(pHandle, (LPVOID)(base + 0x7A774), DISABLE_GUNDRAW_PATCH_BYTES.data(), DISABLE_GUNDRAW_PATCH_BYTES.size(), nullptr);
	}
}

//r_fog
void CustomCommands::toggleFog(bool b) {
	constexpr std::array<unsigned char, 5> DISABLE_FOG_PATCH_BYTES = { 0x90, 0x90, 0x90, 0x90, 0x90 }; //patch
	constexpr std::array<unsigned char, 5> ENABLE_FOG_PATCH_BYTES = { 0xE8, 0x02, 0x4B, 0x46, 0x00 }; //original
	HANDLE pHandle = GetCurrentProcess();
	if (b) {
		WriteProcessMemory(pHandle, (LPVOID)(base + 0x47B19), ENABLE_FOG_PATCH_BYTES.data(), ENABLE_FOG_PATCH_BYTES.size(), nullptr);
	}
	else {
		WriteProcessMemory(pHandle, (LPVOID)(base + 0x47B19), DISABLE_FOG_PATCH_BYTES.data(), DISABLE_FOG_PATCH_BYTES.size(), nullptr);
	}
}