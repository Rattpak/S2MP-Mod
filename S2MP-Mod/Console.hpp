#pragma once
#include <string>
#include <vector>
#include <windows.h>

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

class InternalConsole {
public:
	static void init();
};

class ExtConsole {
public:
	static void extConInit(int extConsoleMode);
	static void coutInfo(const std::string& s);
	static void coutCustom(const std::string& s, const std::string& s2);
	static void coutInit(const std::string& s);
private:
	static void consoleMainLoop();
};

class ExternalConsoleGui {
public:
	static int init(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow);
	static void print(const std::string& s);
	static bool isExtConGuiReady();
};