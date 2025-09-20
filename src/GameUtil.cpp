/////////////////////////////////////////
//           Game Util
//	Utility functions for the mod
/////////////////////////////////////////
#include "pch.h"
#include "GameUtil.hpp"
#include "FuncPointers.h"
#include "Console.hpp"
#include <sstream>
#include <algorithm>
#include <list>

typedef unsigned int uint32;
char** commandTextBuffers = reinterpret_cast<char**>(0xAC664B8_b);

std::string GameUtil::sanitizeFileName(const std::string& name) {
    std::string safe = name;
    for (char& c : safe) {
        if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }
    return safe;
}

void GameUtil::Cbuf_AddText(LocalClientNum_t localClientNum, const std::string& command) {
    int bufferIndex = Functions::_GetAvailableCommandBufferIndex();
    if (bufferIndex == -1) {
        return; 
    }

    Functions::_Sys_EnterCriticalSection(193);

    char** commandBuffer = &commandTextBuffers[2 * bufferIndex];
    uint32_t currentOffset = *(reinterpret_cast<uint32_t*>(commandBuffer) + 3);
    uint32_t bufferSize = *(reinterpret_cast<uint32_t*>(commandBuffer) + 2);

    std::string commandWithNewline = command + "\n";
    size_t commandLength = commandWithNewline.length();

    if (currentOffset + commandLength < bufferSize) {
        strcpy_s(&(*commandBuffer)[currentOffset], bufferSize - currentOffset, commandWithNewline.c_str());
        *(reinterpret_cast<uint32_t*>(commandBuffer) + 3) += static_cast<uint32_t>(commandLength);
    }

    Functions::_Sys_LeaveCriticalSection(193);
}

float GameUtil::safeStringToFloat(const std::string& str) {
    try {
        size_t pos;
        float result = std::stof(str, &pos);
        if (pos != str.length()) {
            return 0.0f;
        }
        return result;
    }
    catch (const std::exception&) {
        return 0.0f;
    }
}

int GameUtil::safeStringToInt(const std::string& str) {
    try {
        size_t pos;
        int result = std::stoi(str, &pos);
        if (pos != str.length()) {
            return 0;
        }
        return result;
    }
    catch (const std::exception&) {
        return 0;
    }
}

//Get pointer address as an std::string
std::string GameUtil::getAddressAsString(void* address) {
    std::stringstream ss;
    ss << address;
    return ss.str();
}

//Use with std::transform
char GameUtil::asciiToLower(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

bool GameUtil::stringToBool(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), GameUtil::asciiToLower);

    return (lowerStr == "1" || lowerStr == "true");
}


std::list<cmd_function_s> cmdHeap;

void GameUtil::addCommand(char const* name, void (*func)()) {
    cmdHeap.emplace_back();
    auto it = std::prev(cmdHeap.end());
    Functions::_Cmd_AddCommandInternal(name, func, &(*it));
}