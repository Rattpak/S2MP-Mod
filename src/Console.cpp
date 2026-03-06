///////////////////////////////////////
//             Console
// Logic and Util for int and ext con
///////////////////////////////////////
#include "pch.h"
#include "Console.hpp"
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <array>
#include "FuncPointers.h"
#include "structs.h"
#include <regex>
#include "GameUtil.hpp"
#include "Noclip.hpp"
#include "DvarInterface.hpp"
#include "DevDef.h"
#include <Arxan.hpp>
#include <Loaders.hpp>

//Output to internal console without label
void Console::printIntCon(std::string text) {
	//Internal Console
	InternalConsole::addToOutputStack(text, 0);
}


//Output to all consoles without label
void Console::print(const std::string& text) {
	
	// External CLI
	std::cout << text << std::endl;
	// External Console Window
	ExternalConsoleGui::print(text);
	// Internal Console
	InternalConsole::addToOutputStack(text, 0);
}

/**
 * @brief Prints a formatted message to all consoles.
 *
 * Formats the given printf-style format string using the supplied
 * variable arguments and forwards the resulting message to
 * Console::print().
 *
 * @param fmt A printf-style format string.
 * @param ... Additional arguments referenced by the format string.
 */
void Console::printf(const char* fmt, ...) {
	if (!fmt) {
		return;
	}
	va_list args;
	va_start(args, fmt);

	std::vector<char> msgBuf(1024);
	vsnprintf(msgBuf.data(), msgBuf.size(), fmt, args);

	va_end(args);
	std::string finalMsg = msgBuf.data();

	Console::print(finalMsg);
}

//Output to all consoles with a label
void Console::labelPrint(std::string label, std::string text) {
	std::string s = "[" + label + "] " + text;
	//External CLI
	ExtConsole::coutCustom(label, text);
	//External Console Window
	ExternalConsoleGui::print(s);
	//Internal Console
	InternalConsole::addToOutputStack(s, 0);
}

//Output to all consoles as info print
void Console::infoPrint(std::string text) {
	std::string s = "[INFO] " + text;
	//External CLI
	ExtConsole::coutInfo(text);
	//External Console Window
	ExternalConsoleGui::print(s);
	//Internal Console
	InternalConsole::addToOutputStack(s, 0);
}

//TODO: add preprocessor directive for developer like in t6sp-mod
//Output to all consoles as client developer print
void Console::devPrint(std::string text) {
#ifdef DEVELOPMENT_BUILD
	std::string s = "[DEV] " + text;
	//External CLI
	ExtConsole::coutInfo(text);
	//External Console Window
	ExternalConsoleGui::print(s);
	//Internal Console
	InternalConsole::addToOutputStack(s, 0);
#endif
}

//Output to all consoles as initialization print
void Console::initPrint(std::string text) {
	std::string s = "[INIT] " + text;
	//External CLI
	ExtConsole::coutInit(text);
	//External Console Window
	ExternalConsoleGui::print(s);
	//Internal Console
	InternalConsole::addToOutputStack(s, 0);
}

//Parse command string into a vector of strings. Anything inside of quotes will be a single string
std::vector<std::string> Console::parseCmdToVec(const std::string& cmd) {
	std::vector<std::string> components;
	std::regex pattern(R"((\"[^\"]*\"|\S+))");
	auto words_begin = std::sregex_iterator(cmd.begin(), cmd.end(), pattern);
	auto words_end = std::sregex_iterator();

	for (auto it = words_begin; it != words_end; ++it) {
		std::string match = it->str();
		if (match.size() > 1 && match.front() == '"' && match.back() == '"') {
			match = match.substr(1, match.size() - 2);
		}
		components.push_back(match);
	}
	return components;
}

//TODO: move to gameutil
std::string toHex(uint32_t value) {
	std::stringstream ss;
	ss << std::hex << value;
	return ss.str();
}

#ifdef DEVELOPMENT_BUILD
void printfCrashTest() {
	Console::printf("%s %i %s", nullptr, nullptr, 0);
}
#endif

void aatestInfo() {
	DEV_ONLY_FUNCTION();
	int localclientprofile = 0;
	uint8_t* base = reinterpret_cast<uint8_t*>(0x8DD0C90_b);
	uint8_t* profile = base + (6920 * localclientprofile);
	bool autoAim = profile[120] != 0;
	bool aimAssistLockon = profile[275] != 0;
	bool aimAssistSlowdown = profile[276] != 0;

	Console::printf("Auto Aim (AutoAim): %s", autoAim ? "ON" : "OFF");
	Console::printf("Aim Assist Rotation (AimAssistLockon): %s", aimAssistLockon ? "ON" : "OFF");
	Console::printf("Aim Assist ??? (AimAssistSlowdown): %s", aimAssistSlowdown ? "ON" : "OFF");
}

void aatestOn() {
	DEV_ONLY_FUNCTION();
	//TODO: use CL_IsSplitscreenControllerActive when calculating these!
	int localclientprofile = 0;
	uint8_t* base = reinterpret_cast<uint8_t*>(0x8DD0C90_b);
	uint8_t* profile = base + (6920 * localclientprofile);
	profile[120] = 1;//autoAim
	profile[275] = 1; //aimAssistLockon
	profile[276] = 1; //aimAssistSlowdown
}

void dumpTable() {
	DEV_ONLY_FUNCTION();
	CmdArgs* cmdArgs = GameUtil::getCmdArgs();
	if (!cmdArgs) {
		return;
	}

	int nest = cmdArgs->nesting;
	int count = cmdArgs->argc[nest];
	if (count != 2) {
		Console::printf("invalid params");
		return;
	}

	const char** args = cmdArgs->argv[nest];

	StringTable* table;
	Functions::_StringTable_GetAsset(args[1], &table);
	if (table) {
		StringTableLoader::dump(table);
	}
	else {
		Console::printf("does not exist");
	}
	
}

void printSupplyDropInfo() {
	DEV_ONLY_FUNCTION();
	const char* str = reinterpret_cast<const char*>(0x89D5CC0_b);
	Console::printf(str);
}

//super temp
#ifdef DEVELOPMENT_BUILD
void dumpAssetNames() {
	uintptr_t g_assetNamesAddr = (uintptr_t)0x10A02B0_b;
	const int MAX_ASSET_TYPES = 0x53;

	const char** assetNames = reinterpret_cast<const char**>(g_assetNamesAddr);

	for (int i = 0; i < MAX_ASSET_TYPES; ++i) {
		const char* name = assetNames[i];
		if (!name) {
			break;
		}

		Console::printf("%s = 0x%X", name, i);
	}
}
#endif // DEVELOPMENT_BUILD



void Console::registerCustomCommands() {
	GameUtil::addCommand("noclip", &Noclip::toggle);
	GameUtil::addCommand("map_restart", &CustomCommands::mapRestart);
	GameUtil::addCommand("fast_restart", &CustomCommands::fastRestart);
	GameUtil::addCommand("god", &CustomCommands::toggleGodmode);
	GameUtil::addCommand("trans", &CustomCommands::translateString);
	GameUtil::addCommand("luidbg", &DevDraw::toggleLuaDebugGui);
	GameUtil::addCommand("entdbg", &DevDraw::toggleEntityDebugGui);
	GameUtil::addCommand("acdbg", &DevDraw::toggleAntiCheatDebugGui);
	GameUtil::addCommand("intcondbg", &DevDraw::toggleIntConDebugGui);
	GameUtil::addCommand("listcmd", &CustomCommands::listAllCmds);
	GameUtil::addCommand("map", &CustomCommands::changeMap);
	GameUtil::addCommand("cmdtest", &CustomCommands::cmdTest);
	GameUtil::addCommand("quit", &CustomCommands::quit);
	GameUtil::addCommand("clear", &InternalConsole::clearFullConsole);
	GameUtil::addCommand("r_fullbright", &CustomCommands::tempToggleFullbright);
	GameUtil::addCommand("r_wireframe", &CustomCommands::tempToggleWireframe);
	GameUtil::addCommand("listassetpool", &CustomCommands::listAssetPool);
	GameUtil::addCommand("saveassetpool", &CustomCommands::saveAssetPool);
#ifdef DEVELOPMENT_BUILD

	GameUtil::addCommand("dumpAssetNames", &dumpAssetNames);
	GameUtil::addCommand("printfNullptr", &printfCrashTest);
	GameUtil::addCommand("aaInfo", &aatestInfo);
	GameUtil::addCommand("aaOn", &aatestOn);
	GameUtil::addCommand("dumpTable", &dumpTable);
	GameUtil::addCommand("printSupplyDropInfo", &printSupplyDropInfo);
	GameUtil::addCommand("prevmat", &CustomCommands::previewMaterial);
	GameUtil::addCommand("imgtest", &DevPatches::imgtest);
#endif // DEVELOPMENT_BUILD

}



void Console::registerCustomDvars() {
#ifdef DEVELOPMENT_BUILD
	DvarInterface::registerBool("testBool", 1, 0, "S2MP-Mod custom bool test");
	DvarInterface::registerBool("unlockAllItems", 1, 0, "S2MP-Mod unlock all items test. Once this shit is on you aint turning it off without a restart.");
#endif // DEVELOPMENT_BUILD
	DvarInterface::registerBool("g_dumpLui", 0, 0, "Dump LUI files on map load");
	DvarInterface::registerBool("g_dumpStringTables", 0, 0, "Dump StringTables when they are loaded");
	DvarInterface::registerBool("g_dumpRawfiles", 0, 0, "Dump RawFiles when they are loaded");
	DvarInterface::registerBool("g_dumpScripts", 0, 0, "Toggle dumping scripts on map load");
	DvarInterface::registerBool("printWorldInfo", 0, 0, "Prints GfxWorld build info on load");
	DvarInterface::registerBool("g_dumpMapEnts", 0, 0, "Dump MapEnts when they are loaded"); //TODO
	//DvarInterface::registerBool("r_fullbright", 0, 0, "Toggles rendering without lighting");
	DvarInterface::registerBool("cg_hudblood", 1, 0, "Enable drawing of on-screen damage blood");
	DvarInterface::registerBool("r_fog", 1, 0, "Set to 0 to disable fog");
}

//useful for testing commands and handling non-cmd/non-dvar stuff
bool execCustomDevCmd(std::string& cmd) {
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), GameUtil::asciiToLower);
	std::vector<std::string> p = Console::parseCmdToVec(cmd);

#ifdef DEVELOPMENT_BUILD
	//--------------------TEMP--------------------
	if (p[0] == "trans") {
		if (p.size() == 2) {
			Console::print("Translated String: " + std::string(Functions::_SEH_SafeTranslateString(p[1].c_str())));
		}
		return true;
	}

	if (p[0] == "send") {
		if (p.size() == 2) {
			std::string str = "%c \"" + p[1] + "\"";
			Functions::_SV_SendServerCommand(0i64, 0, str.c_str(), 101i64);
		}
		return true;
	}
	//--------------------------------------------
#endif // DEVELOPMENT_BUILD

	
	//TODO: Maybe find a way to better setup these one-shot dvars.
	if (p[0] == "cg_hudblood") {
		if (p.size() >= 2) {
			bool val = GameUtil::stringToBool(p[1]);
			CustomCommands::toggleHudBlood(val);
			Functions::_Dvar_FindVar("cg_hudblood")->current.enabled = val;
		}
		return true;
	}

	if (p[0] == "r_fog") {
		if (p.size() >= 2) {
			bool val = GameUtil::stringToBool(p[1]);
			CustomCommands::toggleFog(GameUtil::stringToBool(p[1]));
			Functions::_Dvar_FindVar("r_fog")->current.enabled = val;
		}
		return true;
	}

	//this can and will desync so easily
	//if (p[0] == "r_fullbright") {
	//	if (p.size() >= 2) {
	//		bool val = GameUtil::stringToBool(p[1]);
	//		CustomCommands::toggleFullbright(val);
	//		Functions::_Dvar_FindVar("r_fullbright")->current.enabled = val;
	//	}
	//	return true;
	//}

	return false;
}

//Formats a commands and sends it to the dvar interface. Returns true if successful
bool setEngineDvar(std::string cmd) {
	std::vector<std::string> p = Console::parseCmdToVec(cmd);
	return DvarInterface::setDvar(p[0], p);
}

//All consoles use this to execute commands. 
void Console::execCmd(std::string cmd) {
	if (cmd.length() == 0) {
		return;
	}
	if (!execCustomDevCmd(cmd) && !setEngineDvar(cmd)) {
		Console::printIntCon(cmd);
		GameUtil::Cbuf_AddText(LOCAL_CLIENT_0, (char*)cmd.c_str());
	}
}