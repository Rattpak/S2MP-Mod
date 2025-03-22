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

void GameUtil::Cbuf_AddText(LocalClientNum_t localClientNum, std::string text) {
    //TODO: add this
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