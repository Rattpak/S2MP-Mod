#pragma once
#include <string>
#include <vector>
class Console {
public:
	static void execCmd(std::string cmd);
	static void print(std::string text);
	static void labelPrint(std::string label, std::string text);
	static void infoPrint(std::string text);
	static void devPrint(std::string text);
	static void initPrint(std::string text);
	static std::vector<std::string> parseCmdToVec(const std::string& cmd);
};