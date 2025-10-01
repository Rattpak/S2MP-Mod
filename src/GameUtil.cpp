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
#include "resource.h"

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

bool GameUtil::bytesMatch(const uint8_t* addr, const std::initializer_list<uint8_t>& pattern) {
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (addr[i] != *(pattern.begin() + i)) {
            return false;
        }
    }
    return true;
}


void GameUtil::setCustomSplashScreen() {

}

/*
    Enable/Disable keycatching for the game. Used for internal console
*/
void GameUtil::blockGameInput(bool b) {
    if (b) {
        int* pKeyCatchers = (int*)0x1C65F20_b;
        *pKeyCatchers |= 1;
    }
    else {
        int* pKeyCatchers = (int*)0x1C65F20_b;
        *pKeyCatchers &= ~1;
    }
}

std::string GameUtil::colorToString(const unsigned __int8 color[4]) {
    std::ostringstream oss;
    for (int i = 0; i < 4; ++i) {
        oss << std::fixed << std::setprecision(2) << (color[i] * (1.0f / 255.0f));
        if (i < 3) {
            oss << " ";
        }
    }
    return oss.str();
}

std::string GameUtil::dvarValueToString(const dvar_t* dvar, bool showQuotesAroundStrings, bool truncateFloats) {
    if (dvar->type == DVAR_TYPE_BOOL || dvar->type == DVAR_TYPE_BOOL_AGAIN) {
        return dvar->current.enabled ? "1" : "0";
    }
    if (dvar->type == DVAR_TYPE_FLOAT) {
        if (truncateFloats) {
            std::ostringstream stream;
            stream << std::fixed << std::setprecision(1) << dvar->current.value;
            std::string result = stream.str();
            return result;
        }
        return std::to_string(dvar->current.value);
    }
    if (dvar->type == DVAR_TYPE_FLOAT_2) {
        return std::to_string(dvar->current.value) + " " + std::to_string(dvar->current.vector[1]);
    }
    if (dvar->type == DVAR_TYPE_FLOAT_3) {
        return std::to_string(dvar->current.value) + " " + std::to_string(dvar->current.vector[1]) + " " + std::to_string(dvar->current.vector[2]);
    }
    if (dvar->type == DVAR_TYPE_FLOAT_4) {
        return std::to_string(dvar->current.value) + " " + std::to_string(dvar->current.vector[1]) + " " + std::to_string(dvar->current.vector[2]) + " " + std::to_string(dvar->current.vector[3]);
    }
    if (dvar->type == DVAR_TYPE_INT || dvar->type == DVAR_TYPE_COUNT) {
        return std::to_string(dvar->current.integer);
    }
    if (dvar->type == DVAR_TYPE_ENUM) {
        //if (!dvar->domain.enumeration.stringCount) {
            return "PLACEHOLDER: ENUM";
        //}
        //else {
            //for (int i = 0; i < dvar->domain.enumeration.stringCount; i++) {
            //	possible += dvar->domain.enumeration.strings[i];
            //	if (i + 1 != dvar->domain.enumeration.stringCount) {
            //		possible += ", ";
            //	}
            //}
          //  return *(const char**)(dvar->domain.integer.max + 4 * dvar->current.integer);
       // }
    }
    if (dvar->type == DVAR_TYPE_FLOAT_SPECIAL) {
        if (truncateFloats) {
            std::ostringstream stream;
            stream << std::fixed << std::setprecision(1) << dvar->current.floatSpecial.value;
            std::string result = stream.str();
            return result;
        }
        return std::to_string(dvar->current.floatSpecial.value);
    }
    if (dvar->type == DVAR_TYPE_STRING) {
        if (showQuotesAroundStrings) {
            return "\"" + std::string(dvar->current.string) + "\"";
        }
        return dvar->current.string;
    }
    if (dvar->type == DVAR_TYPE_COLOR || dvar->type == DVAR_TYPE_COLOR2) {
        return colorToString(dvar->current.color);
    }

    return "Unsupported type: " + std::to_string(dvar->type);
}

std::string GameUtil::toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

std::string GameUtil::toUpper(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool GameUtil::isOnlyWhitespace(const std::string& str) {
    std::string noSpaces;
    noSpaces.reserve(str.size()); //avoid reallocs

    std::remove_copy_if(
        str.begin(), str.end(),
        std::back_inserter(noSpaces),
        [](unsigned char c) { return std::isspace(c); }
    );

    return noSpaces.empty();
}

std::string GameUtil::getStringFromClipboard() {
    //open clipboard
    if (!OpenClipboard(nullptr)) {
        Console::print("Cannot open the clipboard");
        return "";
    }

    //make sure clipboard has text
    if (!IsClipboardFormatAvailable(CF_TEXT)) {
        Console::print("Clipboard does not contain text data");
        CloseClipboard();
        return "";
    }

    //get handle to clipboard data based text format
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        Console::print("Cannot get clipboard data");
        CloseClipboard();
        return "";
    }

    //lock handle to get actual text ptr
    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == nullptr) {
        Console::print("Cannot lock global handle");
        CloseClipboard();
        return "";
    }
    std::string text = pszText;
    GlobalUnlock(hData);
    CloseClipboard();

    //only paste upto newline
    size_t newlinePos = text.find('\n');
    if (newlinePos != std::string::npos) {
        text = text.substr(0, newlinePos);
    }
    //Console::print(text);

    return text;
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


std::list<cmd_function_s> GameUtil::cmdHeap;

void GameUtil::addCommand(char const* name, void (*func)()) {
    cmdHeap.emplace_back();
    auto it = std::prev(cmdHeap.end());
    Functions::_Cmd_AddCommandInternal(name, func, &(*it));
}

//remove field widths like %10d from fmt strings
std::string GameUtil::sanitizeFormatWidths(const char* fmt) {
    std::string s(fmt);
    return std::regex_replace(s, std::regex("%(\\d+)([a-zA-Z])"), "%$2");
}