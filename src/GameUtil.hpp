#pragma once
#include <string>
#include <vector>
#include "client/game/structs.h"
class GameUtil {
public:
	static std::string sanitizeFileName(const std::string& name);
	//static void Cbuf_AddText(LocalClientNum_t localClientNum, std::string text);
	static void Cbuf_AddText(LocalClientNum_t localClientNum, const std::string& command);
	static float safeStringToFloat(const std::string& str);
	static int safeStringToInt(const std::string& str);
	static std::string getAddressAsString(void* address);
	static char asciiToLower(char in);
	static bool stringToBool(const std::string& str);
	static void addCommand(char const* name, void (*func)());
};