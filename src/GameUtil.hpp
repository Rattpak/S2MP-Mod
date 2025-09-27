#pragma once
#include <string>
#include <vector>
#include "structs.h"
class GameUtil {
public:
	static std::string sanitizeFileName(const std::string& name);
	static void blockGameInput(bool b);
	static std::string colorToString(const unsigned __int8 color[4]);
	static std::string dvarValueToString(const dvar_t* dvar, bool showQuotesAroundStrings, bool truncateFloats);
	static std::string toLower(const std::string& str);
	static std::string toUpper(const std::string& input);
	static bool isOnlyWhitespace(const std::string& str);
	static std::string getStringFromClipboard();
	//static void Cbuf_AddText(LocalClientNum_t localClientNum, std::string text);
	static void Cbuf_AddText(LocalClientNum_t localClientNum, const std::string& command);
	static float safeStringToFloat(const std::string& str);
	static int safeStringToInt(const std::string& str);
	static std::string getAddressAsString(void* address);
	static char asciiToLower(char in);
	static bool stringToBool(const std::string& str);
	static void addCommand(char const* name, void (*func)());

	static std::string sanitizeFormatWidths(const char* fmt);

	static std::list<cmd_function_s> cmdHeap;
};