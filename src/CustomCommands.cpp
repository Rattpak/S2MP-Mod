//////////////////////////////////////
//       Custom Commands
//	Functions for custom commands
//////////////////////////////////////
#include "pch.h"
#include "Console.hpp"
#include "FuncPointers.h"
#include <array>
#include "DvarInterface.hpp"
#include "GameUtil.hpp"

uintptr_t CustomCommands::base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;
uintptr_t CustomCommands::rawBase = (uintptr_t)GetModuleHandle(NULL);
bool CustomCommands::isGodmode = false;

void CustomCommands::toggleGodmode() {
	int* health = (int*)(rawBase + 0xA0C740C);
	if (CustomCommands::isGodmode) {
		*health = 100;
		Console::print("God: OFF");
		Functions::_SV_SendServerCommand(0i64, 0, "%c \"God: ^1OFF\"", 101i64);
	}
	else {
		*health = -1;
		Console::print("God: ON");
		Functions::_SV_SendServerCommand(0i64, 0, "%c \"God: ^2ON\"", 101i64);
	}
	CustomCommands::isGodmode = !CustomCommands::isGodmode;
}


void CustomCommands::listAllCmds() {
	cmd_function_s* cmd = *(cmd_function_s**)0xAC662D8_b;
	while (cmd) {
		if (IsBadReadPtr(cmd, sizeof(cmd_function_s))) {
			break;
		}


		if (cmd->name && !IsBadStringPtrA(cmd->name, 64)) {
			Console::print(std::string(cmd->name));
		}
		cmd = cmd->next;
	}
}

//cg_hudblood
void CustomCommands::toggleHudBlood(bool b) {
	constexpr std::array<unsigned char, 5> DISABLE_HUDBLOOD_PATCH_BYTES = { 0x90, 0x90, 0x90, 0x90, 0x90 }; //patch
	constexpr std::array<unsigned char, 5> ENABLE_HUDBLOOD_PATCH_BYTES = { 0xE8, 0xF6, 0x4D, 0xFE, 0xFF }; //original
	HANDLE pHandle = GetCurrentProcess();
	if (b) {
		WriteProcessMemory(pHandle, (LPVOID)(base + 0x5DCE5), ENABLE_HUDBLOOD_PATCH_BYTES.data(), ENABLE_HUDBLOOD_PATCH_BYTES.size(), nullptr);
	}
	else {
		WriteProcessMemory(pHandle, (LPVOID)(base + 0x5DCE5), DISABLE_HUDBLOOD_PATCH_BYTES.data(), DISABLE_HUDBLOOD_PATCH_BYTES.size(), nullptr);
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

void CustomCommands::translateString() {

}

void CustomCommands::cmdTest() {
	CmdArgs* cmdArgs = GameUtil::getCmdArgs();
	if (!cmdArgs) {
		return;
	}

	int nest = cmdArgs->nesting;
	int count = cmdArgs->argc[nest];
	Console::printf("CmdArgs (nesting = %d) has %d arguemnts:", nest, count);

	const char** args = cmdArgs->argv[nest];

	for (int i = 0; i < count; ++i) {
		if (!IsBadStringPtrA(args[i], 64)) {
			Console::printf("Arg[%d]: %s", i, args[i]);
		}
	}
}

//when it works, causes too many lui errors in main.lua
//TODO: fix
void CustomCommands::changeMap() {
	bool isMapPreloaded = false;
	CmdArgs* cmdArgs = GameUtil::getCmdArgs();
	if (!cmdArgs) {
		return;
	}

	int nest = cmdArgs->nesting;
	int count = cmdArgs->argc[nest];
	if (count < 2) {
		Console::print("Usage: map <map name> <optional sv_migrate>");
		return;
	}

	if (count <= 3) {
		int* sv_migrate = (int*)0xBD44248_b;
		*sv_migrate = 0;
	}
	else {
		int* sv_migrate = (int*)0xBD44248_b;
		*sv_migrate = atol(cmdArgs->argv[nest][3]); //TODO: make this safe
	}
	
	Functions::_SV_StartMap(LOCAL_CLIENT_0, cmdArgs->argv[nest][2], isMapPreloaded);
}

void CustomCommands::fastRestart() {
	int* sv_map_restart = (int*)(base + 0xBD44240);
	*sv_map_restart = 1;
}

void CustomCommands::mapRestart() {
	int* sv_loadScripts = (int*)(base + 0xBD44244);
	*sv_loadScripts = 1;
	int* sv_map_restart = (int*)(base + 0xBD44240);
	*sv_map_restart = 1;
}

void CustomCommands::quit() {
	Console::print("quitting...");
	Functions::_Com_Quit_f();
	//TODO: delete the __s2Exe file here
}