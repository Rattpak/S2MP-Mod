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


void Console::Print(printType type, const char* fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);

	char temp[500];

	vsnprintf(temp, sizeof(temp), fmt, argList);
	va_end(argList);

	char* buffer = (char*)malloc(strlen(temp) + 1);
	if (buffer) {
		strcpy(buffer, temp);
	}

	switch (type)
	{
	case info:
		Console::labelPrint("INFO", buffer);
		break;
	case error:
		Console::labelPrint("ERROR", buffer);
		break;
	case dev:
		Console::labelPrint("DEV", buffer);
		break;
	default:
		Console::labelPrint("", buffer);
	}
}


//Output to all consoles without label
void Console::print(std::string text) {
	//External CLI
	std::cout << text << std::endl;
	//External Console Window
	ExternalConsoleGui::print(text);
	//Internal Console
	InternalConsole::addToOutputStack(text, 0);
}

void Console::printf(const char* fmt, ...) {
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
	std::string s = "[DEV] " + text;
	//External CLI
	ExtConsole::coutInfo(text);
	//External Console Window
	ExternalConsoleGui::print(s);
	//Internal Console
	InternalConsole::addToOutputStack(s, 0);
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

void Console::registerCustomCommands() {
	GameUtil::addCommand("noclip", &Noclip::toggle);
	GameUtil::addCommand("map_restart", &CustomCommands::mapRestart);
	GameUtil::addCommand("fast_restart", &CustomCommands::fastRestart);
	GameUtil::addCommand("god", &CustomCommands::toggleGodmode);
	GameUtil::addCommand("trans", &CustomCommands::translateString);
	GameUtil::addCommand("luidbg", &DevDraw::toggleLuaDebugGui);
	GameUtil::addCommand("entdbg", &DevDraw::toggleEntityDebugGui);
	GameUtil::addCommand("acdbg", &DevDraw::toggleAntiCheatDebugGui);
}

//useful for testing commands and handling non-cmd/non-dvar stuff
bool execCustomDevCmd(std::string& cmd) {
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), GameUtil::asciiToLower);
	std::vector<std::string> p = Console::parseCmdToVec(cmd);

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
	
	if (p[0] == "up") {
		Functions::_LiveStorage_UploadStats(0);
		return true;
	}
	
	if (p[0] == "ftest") {
		Functions::_R_RegisterFont("testfakefont.ttf", 16);
		return true;
	}
	//--------------------------------------------

	
	if (p[0] == "map") {
		if (p.size() >= 2) {
			CustomCommands::changeMap(p[1]);
		}
		return true;
	}

	
	if (p[0] == "quit") {
		exit(0);
		return true;
	}

	//if (p[0] == "cg_drawlui") {
	//	if (p.size() >= 2) {
	//		CustomCommands::toggleHud(GameUtil::stringToBool(p[1]));
	//	}
	//	return true;
	//}
	
	if (p[0] == "cg_hudblood") {
		if (p.size() >= 2) {
			CustomCommands::toggleHudBlood(GameUtil::stringToBool(p[1]));
		}
		return true;
	}
	
	//TODO: register these as dvar?
	if (p[0] == "r_fog") {
		if (p.size() >= 2) {
			CustomCommands::toggleFog(GameUtil::stringToBool(p[1]));
		}
		return true;
	}
	
	//if (p[0] == "cg_drawgun") {
	//	if (p.size() >= 2) {
	//		CustomCommands::toggleGun(GameUtil::stringToBool(p[1]));
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
		Console::print(cmd); //TODO: change this to only print to internal console
		GameUtil::Cbuf_AddText(LOCAL_CLIENT_0, (char*)cmd.c_str());
	}

}