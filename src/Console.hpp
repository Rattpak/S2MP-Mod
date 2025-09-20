#pragma once
#include <string>
#include <vector>
#include <windows.h>

class Console {
public:
	enum printType
	{
		info,
		error,
		dev,
	};

	static void Print(printType type, const char* fmt, ...);
	static void execCmd(std::string cmd);
	static void print(std::string text);
	static void printf(const char* fmt, ...);
	static void labelPrint(std::string label, std::string text);
	static void infoPrint(std::string text);
	static void devPrint(std::string text);
	static void initPrint(std::string text);
	static std::vector<std::string> parseCmdToVec(const std::string& cmd);
	static void registerCustomCommands();
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

class CustomCommands {
public:
	static uintptr_t base;
	static uintptr_t rawBase;
	static void toggleGodmode();
	static void toggleHud(bool b);
	static void toggleHudBlood(bool b);
	static void toggleGun(bool b);
	static void toggleFog(bool b);
	static void translateString();
	static void changeMap(const std::string& mapname);
	static void fastRestart();
	static void mapRestart();
private:
	static bool isGodmode;
};