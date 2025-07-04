///////////////////////////////////////
//             Console
// Logic and Util for int and ext con
///////////////////////////////////////
#include "pch.h"
#include "Console.hpp"
#include <string>
#include <algorithm>
#include "ExtConsole.hpp"
#include <iostream>
#include "ExtConsoleGui.hpp"
#include <sstream>
#include <array>
#include "FuncPointers.h"
#include "structs.h"
#include <regex>
#include "GameUtil.hpp"
#include "Noclip.hpp"
#include "CustomCommands.hpp"
#include "DvarInterface.hpp"

//Output to all consoles without label
void Console::print(std::string text) {
	//External CLI
	std::cout << text << std::endl;
	//External Console Window
	ExternalConsoleGui::print(text);
	//Internal Console
	//.....
}

//Output to all consoles with a label
void Console::labelPrint(std::string label, std::string text) {
	std::string s = "[" + label + "] " + text;
	//External CLI
	ExtConsole::coutCustom(label, text);
	//External Console Window
	ExternalConsoleGui::print(s);
	//Internal Console
	//.....
}

//Output to all consoles as info print
void Console::infoPrint(std::string text) {
	std::string s = "[INFO] " + text;
	//External CLI
	ExtConsole::coutInfo(text);
	//External Console Window
	ExternalConsoleGui::print(s);
	//Internal Console
	//.....
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
	//.....
}

//Output to all consoles as initialization print
void Console::initPrint(std::string text) {
	std::string s = "[INIT] " + text;
	//External CLI
	ExtConsole::coutInit(text);
	//External Console Window
	ExternalConsoleGui::print(s);
	//Internal Console
	//.....
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

//Gonna have to run the commands externally like this for now
bool execCustomCmd(std::string& cmd) {
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
	
	if (p[0] == "uicmd") {
		if (p.size() == 2) {
			if (Functions::_SV_Loaded()) {
				const char* args = p[1].c_str();
				Functions::_UI_RunMenuScript(0, &args);
			}
			else {
				Console::print("cmd failed: Server not loaded");
			}
		}
		return true;
	}
	
	if (p[0] == "isload") {
		if (Functions::_SV_Loaded()) {
			Console::print("Server is loaded");
		}
		else {
			Console::print("Server not loaded");
		}
		return true;
	}
	
	if (p[0] == "windim") {
		int winX = *(int*)(GameUtil::base + 0x1CFF748); //this isnt actually window size i think its bink player size
		int winY = *(int*)(GameUtil::base + 0x1CFF74C);
		Console::devPrint(std::to_string(winX) + "x" + std::to_string(winY));
		return true;
	}
	//--------------------------------------------

	if (p[0] == "noclip") {
		Noclip::toggle();
		return true;
	}
	
	if (p[0] == "map_restart") {
		CustomCommands::mapRestart();
		return true;
	}
	
	if (p[0] == "fast_restart") {
		CustomCommands::fastRestart();
		return true;
	}
	
	if (p[0] == "map") {
		if (p.size() >= 2) {
			CustomCommands::changeMap(p[1]);
		}
		return true;
	}

	if (p[0] == "god") {
		CustomCommands::toggleGodmode();
		return true;
	}
	
	if (p[0] == "quit") {
		Functions::_Sys_Quit();
		return true;
	}

	if (p[0] == "cg_drawlui") {
		if (p.size() >= 2) {
			CustomCommands::toggleHud(GameUtil::stringToBool(p[1]));
		}
		return true;
	}
	
	if (p[0] == "cg_hudblood") {
		if (p.size() >= 2) {
			CustomCommands::toggleHudBlood(GameUtil::stringToBool(p[1]));
		}
		return true;
	}
	
	if (p[0] == "r_fog") {
		if (p.size() >= 2) {
			CustomCommands::toggleFog(GameUtil::stringToBool(p[1]));
		}
		return true;
	}
	
	if (p[0] == "cg_drawgun") {
		if (p.size() >= 2) {
			CustomCommands::toggleGun(GameUtil::stringToBool(p[1]));
		}
		return true;
	}
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
	if (!execCustomCmd(cmd) && !setEngineDvar(cmd)) {
		//Console::devPrint("Passing cmd to command buffer");
		GameUtil::Cbuf_AddText(LOCAL_CLIENT_0, (char*)cmd.c_str());
	}

}