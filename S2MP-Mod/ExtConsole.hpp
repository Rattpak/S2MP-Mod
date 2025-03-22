#pragma once
#include <string>
class ExtConsole {
public:
	static void extConInit(int extConsoleMode);
	static void coutInfo(const std::string& s);
	static void coutCustom(const std::string& s, const std::string& s2);
	static void coutInit(const std::string& s);
private:
	static void consoleMainLoop();
};