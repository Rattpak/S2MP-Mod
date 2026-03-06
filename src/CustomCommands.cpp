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
#include <DevDef.h>

#define ASSET_ENTRY_SIZE 32
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

void DB_ListAssetPool(unsigned int targetType, bool saveToFile) {
	unsigned int count = 0;
	unsigned int totalSize = 0;

	auto g_assetNames = (const char**)0x10A02B0_b;
	auto db_hashTable = (uint32_t*)0x28EC090_b;
	auto g_assetEntryPool = (uint8_t*)0x5211050_b;

	constexpr uint32_t HASH_SIZE = 0x48800; //this looks right from ida
	constexpr uint32_t MAX_ENTRIES = 0x200000; //placeholder

	if (targetType >= 0x53) {
		Console::printf("Invalid asset type %u", targetType);
		return;
	}

	const char* typeName = g_assetNames[targetType];

	std::ofstream outFile;
	if (saveToFile) {
		std::string filename = "assetpool_" + std::to_string(targetType) + ".txt";
		outFile.open(filename, std::ios::out);
		if (!outFile.is_open()) {
			Console::printf("Failed to open file %s for writing.", filename.c_str());
			saveToFile = false;
		}
	}

	if (!saveToFile) {
		Console::printf("Listing assets in %s pool.", typeName);
	}

	for (uint32_t hash = 0; hash < HASH_SIZE; ++hash) {
		uint32_t index = db_hashTable[hash];

		while (index) {
			if (index >= MAX_ENTRIES) {
				break;
			}

			uint8_t* entry = g_assetEntryPool + (index * 32);

			uint32_t assetType = *(uint32_t*)(entry + 0);
			uint32_t nextIndex = *(uint32_t*)(entry + 20); //entry[5]

			if (assetType == targetType) {
				const char* assetName = Functions::_DB_GetXAssetName(entry);
				if (assetName) {
					if (saveToFile) {
						outFile << assetName << std::endl;
					}
					else {
						Console::printf("%s", assetName);
					}
				}

				++count;
				totalSize += Functions::_DB_GetXAssetTypeSize(assetType);
			}

			if (nextIndex >= MAX_ENTRIES) {
				break;
			}

			index = nextIndex;
		}
	}

	if (saveToFile) {
		outFile.close();
		Console::printf("Saved %u assets in %s pool to file.", count, typeName);
	}
	else {
		Console::printf("Total of %u assets in %s pool", count, typeName);
	}
}

void CustomCommands::previewMaterial() {
	CmdArgs* cmdArgs = GameUtil::getCmdArgs();
	if (!cmdArgs) {
		return;
	}

	int nest = cmdArgs->nesting;
	int count = cmdArgs->argc[nest];
	if (count != 2) {
		Console::printf("Usage: prevmat <material>");
		return;
	}

	const char** args = cmdArgs->argv[nest];
	std::string matName = std::string(args[1]);
	if (matName.size() < 2) {
		DevDraw::previewMaterial = nullptr;
		return;
	}

	DevDraw::previewMaterial = Functions::_Material_RegisterHandle(args[1]);
	Console::devPrint("material " + matName + " located at address 0x" + GameUtil::getAddressAsString(DevDraw::previewMaterial));
}

void printLapUsage(bool isSave) {
	if (isSave) {
		Console::print("saveassetpool <poolnumber>: saves a list of all the assets in the specified pool");
	}
	else {
		Console::print("listassetpool <poolnumber>: lists all the assets in the specified pool");
	}
	uintptr_t g_assetNamesAddr = (uintptr_t)0x10A02B0_b;
	const int MAX_ASSET_TYPES = 0x53;

	const char** assetNames = reinterpret_cast<const char**>(g_assetNamesAddr);
	for (int i = 0; i < MAX_ASSET_TYPES; ++i) {
		const char* name = assetNames[i];
		if (!name) {
			break;
		}

		Console::printf("%i %s", i, name);
	}
}

void CustomCommands::listAssetPool() {
	CmdArgs* cmdArgs = GameUtil::getCmdArgs();
	if (!cmdArgs) {
		return;
	}

	int nest = cmdArgs->nesting;
	int count = cmdArgs->argc[nest];
	if (count != 2) {
		printLapUsage(false);
		return;
	}

	const char** args = cmdArgs->argv[nest];
	unsigned int type = static_cast<unsigned int>(std::strtoul(args[1], nullptr, 10));

	DB_ListAssetPool(type, false);
}

void CustomCommands::saveAssetPool() {
	CmdArgs* cmdArgs = GameUtil::getCmdArgs();
	if (!cmdArgs) {
		return;
	}

	int nest = cmdArgs->nesting;
	int count = cmdArgs->argc[nest];
	if (count != 2) {
		printLapUsage(true);
		return;
	}

	const char** args = cmdArgs->argv[nest];
	unsigned int type = static_cast<unsigned int>(std::strtoul(args[1], nullptr, 10));

	DB_ListAssetPool(type, true);
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

void CustomCommands::toggleFullbright(bool enable) {
	constexpr std::byte ENABLE{ 0x08 };
	constexpr std::byte DISABLE{ 0x0F };
	const std::byte value = enable ? ENABLE : DISABLE;
	WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(0x10037464_b), &value, sizeof(value), nullptr);
}

void CustomCommands::toggleWireframe(bool enable) {
	constexpr std::byte ENABLE{ 0x20 };
	constexpr std::byte DISABLE{ 0x0F };
	const std::byte value = enable ? ENABLE : DISABLE;
	WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(0x10037464_b), &value, sizeof(value), nullptr);
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

void CustomCommands::tempToggleFullbright() {
	std::byte current{};
	const auto address = reinterpret_cast<void*>(0x10037464_b);

	if (ReadProcessMemory(GetCurrentProcess(), address, &current, sizeof(current), nullptr)) {
		toggleFullbright(current != std::byte{ 0x08 });
	}
}

void CustomCommands::tempToggleWireframe() {
	std::byte current{};
	const auto address = reinterpret_cast<void*>(0x10037464_b);

	if (ReadProcessMemory(GetCurrentProcess(), address, &current, sizeof(current), nullptr)) {
		toggleWireframe(current != std::byte{ 0x20 });
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
}