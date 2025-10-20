#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include "structs.h"

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
	static void printIntCon(std::string text);
	static void print(const std::string& text);
	static void printf(const char* fmt, ...);
	static void labelPrint(std::string label, std::string text);
	static void infoPrint(std::string text);
	static void devPrint(std::string text);
	static void initPrint(std::string text);
	static std::vector<std::string> parseCmdToVec(const std::string& cmd);
	static void registerCustomCommands();
	static void registerCustomDvars();
};

class InternalConsole {
public:
	static void clearConsole();
	static void closeConsole();
	static void addToOutputStack(std::string s, int level);
	static material_t* getMaterialWhite();
	static void toggleConsole();
	static void toggleFullConsole();
	static void clearFullConsole();
	static void init();

	//for dev intcon debugger
	static bool DEVONLY_isShift();
	static bool DEVONLY_isAlt();
	static bool DEVONLY_isCtrl();
	static bool DEVONLY_fullConsole();
	static bool DEVONLY_consoleOpen();
	static int DEVONLY_maxLines();
	static int DEVONLY_outputStackSeekPos();
	static int DEVONLY_scrollbarBaseX();
	static int DEVONLY_scrollbarTrackHeight();
	static int DEVONLY_sliderHeight();
	static int DEVONLY_outputStackSize();
	static int DEVONLY_sliderOffsetY();
	static int DEVONLY_cmdStackSize();
	static int DEVONLY_cmdStackSeekPos();
	static int DEVONLY_recentKeynum();
	static std::string DEVONLY_autoCompleteSubstring();
	static bool DEVONLY_didGreenForThisText();
	static int DEVONLY_autoCompleteIndex();
	static int DEVONLY_autoCompleteTextSize();
	static bool DEVONLY_isAutoCompleteCycling();

	static font_t* consoleFont;
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
	static void listAllCmds();
	static void toggleHud(bool b);
	static void toggleHudBlood(bool b);
	static void toggleGun(bool b);
	static void toggleFog(bool b);
	static void translateString();
	static void cmdTest();
	static void changeMap();
	static void fastRestart();
	static void mapRestart();
	static void quit();
private:
	static bool isGodmode;
};