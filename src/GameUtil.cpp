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
char** commandTextBuffers;

/**
 * @brief Sanitizes a string for safe use as a file name.
 *
 * Replaces characters that are invalid in file names with underscores.
 *
 * @param name The input file name.
 * @return A sanitized file name string.
 */
std::string GameUtil::sanitizeFileName(const std::string& name) {
    std::string safe = name;
    for (char& c : safe) {
        if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }
    return safe;
}

/**
 * @brief Compares a memory region against a byte pattern.
 *
 * Checks whether the bytes at the given address match the provided
 * initializer-list pattern exactly.
 *
 * @param addr Pointer to the memory to compare.
 * @param pattern The byte pattern to match.
 * @return true if all bytes match; false otherwise.
 */
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

/**
 * @brief Enables or disables game input processing (keyCatchers).
 *
 * Used primarily by the internal console to block game input
 * while accepting keyboard input.
 *
 * @param b true to block game input; false to restore input.
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


/**
 * @brief Converts a color array to a normalized string representation.
 *
 * Converts RGBA color values (0–255) to normalized floats (0.0–1.0)
 * and formats them as a space-separated string.
 *
 * @param color RGBA color array.
 * @return A formatted string representation of the color.
 */
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

/**
 * @brief Converts a dvar value to string.
 *
 * Handles all supported dvar types and formats the output
 *
 * @param dvar Pointer to the dvar.
 * @param showQuotesAroundStrings Whether string values should be quoted.
 * @param truncateFloats Whether floating-point values should be truncated.
 * @return A string representation of the dvar's current value.
 */
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

/**
 * @brief Converts a string to lowercase.
 *
 * @param str The input string.
 * @return A lowercase version of the input string.
 */
std::string GameUtil::toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

/**
 * @brief Converts a string to uppercase.
 *
 * @param input The input string.
 * @return An uppercase version of the input string.
 */
std::string GameUtil::toUpper(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return result;
}

/**
 * @brief Checks whether a string contains only whitespace.
 *
 * @param str The string to check.
 * @return true if the string contains only whitespace; false otherwise.
 */
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

/**
 * @brief Retrieves text from the system clipboard.
 *
 * Reads text from the clipboard, truncating at the first newline
 * character if present. Basically only used for internal console.
 *
 * @return Clipboard text, or an empty string on failure.
 */
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


/**
 * @brief Appends a command to the engine command buffer.
 *
 * Thread-safe wrapper for submitting commands to the engine.
 *
 * @param localClientNum The local client index.
 * @param command The command string to execute.
 */
void GameUtil::Cbuf_AddText(LocalClientNum_t localClientNum, const std::string& command) {
    commandTextBuffers = reinterpret_cast<char**>(0xAC664B8_b);
    int bufferIndex = Functions::_GetAvailableCommandBufferIndex();
    if (bufferIndex == -1) {
        Console::printf("[Cbuf_AddText] No available command buffer");
        return; 
    }

    Functions::_Sys_EnterCriticalSection(193);

    //Console::printf("[Cbuf_AddText] bufferIndex = %d", bufferIndex);
    //Console::printf("[Cbuf_AddText] commandTextBuffers = %p", static_cast<void*>(commandTextBuffers));

    char** commandBuffer = &commandTextBuffers[2 * bufferIndex];

    //Console::printf("[Cbuf_AddText] commandBuffer = %p", static_cast<void*>(commandBuffer));
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


/**
 * @brief Safely converts a string to a float.
 *
 * Returns 0.0f if the conversion fails or if extra characters
 * are present.
 *
 * @param str The input string.
 * @return The converted float value, or 0.0f on failure.
 */
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

/**
 * @brief Safely converts a string to an integer.
 *
 * Returns 0 if the conversion fails or if extra characters
 * are present.
 *
 * @param str The input string.
 * @return The converted integer value, or 0 on failure.
 */
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

/**
 * @brief Converts a pointer address to a string.
 *
 * @param address Pointer to convert.
 * @return A string representation of the address.
 */
std::string GameUtil::getAddressAsString(void* address) {
    std::stringstream ss;
    ss << address;
    return ss.str();
}

/**
 * @brief Converts an ASCII character to lowercase.
 *
 * Intended for use with std::transform.
 *
 * @param in The input character.
 * @return The lowercase equivalent if applicable.
 */
char GameUtil::asciiToLower(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

/**
 * @brief Converts a string to a boolean value.
 *
 * Accepts "1" or "true" (case-insensitive) as true.
 *
 * @param str The input string.
 * @return true if the string represents true; false otherwise.
 */
bool GameUtil::stringToBool(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), GameUtil::asciiToLower);

    return (lowerStr == "1" || lowerStr == "true");
}


std::list<cmd_function_s> GameUtil::cmdHeap;

/**
 * @brief Registers a console command with the engine.
 *
 * @param name The command name.
 * @param func The function to execute.
 */
void GameUtil::addCommand(char const* name, void (*func)()) {
    cmdHeap.emplace_back();
    auto it = std::prev(cmdHeap.end());
    Functions::_Cmd_AddCommandInternal(name, func, &(*it));
}

/**
 * @brief Removes field width specifiers from printf-style format strings.
 *
 * Converts format specifiers like "%10d" into "%d". Very rare use case.
 *
 * @param fmt The input format string.
 * @return A sanitized format string.
 */
std::string GameUtil::sanitizeFormatWidths(const char* fmt) {
    std::string s(fmt);
    return std::regex_replace(s, std::regex("%(\\d+)([a-zA-Z])"), "%$2");
}


/**
 * @brief Retrieves the current command argument structure.
 *
 * @return Pointer to the CmdArgs structure, or nullptr if unavailable.
 */
CmdArgs* GameUtil::getCmdArgs() {
    CmdArgs* cmdArgs = reinterpret_cast<CmdArgs*>(0xAC662E0_b);
    if (!cmdArgs) {
        Console::printf("%s was called but cmdArgs was null!", __FUNCTION__);
        return nullptr;
    }

    return cmdArgs;
}