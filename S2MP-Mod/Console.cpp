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

bool execCustomCmd(std::string& cmd) {
    // Convert the command to lowercase for case-insensitivity
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), GameUtil::asciiToLower);
    std::vector<std::string> p = Console::parseCmdToVec(cmd);

    // Handle the "send" command
    if (p[0] == "send") {
        if (p.size() >= 2) {
            // Join the remaining parts of the message after the command
            std::string message = cmd.substr(cmd.find(" ") + 1);  // Get the entire string after "send"

            // Send the message to the in-game chat
            Functions::_SV_SendServerCommand(0i64, 0, "%c \"%s\"", 101i64, message.c_str());
            Console::print("Message sent to in-game chat: " + message);
        }
        else {
            Console::print("Usage: send <message>");
        }
        return true;
    }

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

    if (p[0] == "sethealth") {
        if (p.size() >= 2) {
            try {
                int healthValue = std::stoi(p[1]);

                CustomCommands::setHealth(healthValue);
            }
            catch (const std::invalid_argument& e) {
                Console::print("Error: Invalid health value. Please specify a valid integer.");
            }
            catch (const std::out_of_range& e) {
                Console::print("Error: Health value is out of range.");
            }
        }
        else {
            Console::print("Please specify a health value (e.g., sethealth 100).");
        }
        return true;
    }

    if (p[0] == "help") {
        // List all available commands
        Console::print("Available commands:");
        Console::print("noclip - Toggle noclip mode");
        Console::print("god - Toggle godmode");
        Console::print("map_restart - Restart the current map");
        Console::print("fast_restart - Restart the game fast");
        Console::print("map <map_name> - Change to a specified map (not done yet)");
        Console::print("sethealth <health_value> - Set player's health to a specific value");
        Console::print("cg_drawlui - Toggle the UI drawing");
        Console::print("cg_hudblood - Toggle blood effects on the HUD");
        Console::print("r_fog - Toggle fog on or off");
        Console::print("cg_drawgun - Toggle the gun drawing on the screen");
        Console::print("quit - Quit the game");

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
