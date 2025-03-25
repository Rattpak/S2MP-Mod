//////////////////////////////////////
//            Ext Console
//	Logic for the external console(s)
//////////////////////////////////////
#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include <sstream>
#include <C:\Users\Tazei\Downloads\S2MP-Mod-master\S2MP-Mod-master\include\MinHook.h>
#include <array>
#include "ExtConsole.hpp"
#include <signal.h>
#include "ExtConsoleGui.hpp"
#include "Console.hpp"
#include <thread>
#include "FuncPointers.h"
#include "DevPatches.hpp"
#include "PrintPatches.hpp"
#include "DebugPatches.hpp"
#include "DvarInterface.hpp"

#pragma comment(lib, "libMinHook.x64.lib")

HANDLE hProcess;
HINSTANCE hInst;

//these prints will be for external console only
void ExtConsole::coutInit(const std::string& s) {
	std::cout << "[INIT] " << s << std::endl;
}

void ExtConsole::coutInfo(const std::string& s) {
	std::cout << "[INFO] " << s << std::endl;
}

void ExtConsole::coutCustom(const std::string& s, const std::string& s2) {
	std::cout << "[" << s << "] " << s2 << std::endl;
}

void ExtConsole::consoleMainLoop() {
	std::string in;
	std::cout << "----------[ S2MP External Console ]----------" << std::endl;
	while (true) {
		std::cout << "> ";
		getline(std::cin, in);
		//Console::execCmd(in);
	}
}

void infoPrintOffsets() {
	uintptr_t s2base = (uintptr_t)GetModuleHandle(NULL);
	uintptr_t s2baseOff = (uintptr_t)GetModuleHandle(NULL) + 0x1000;
	std::ostringstream oss;
	oss << "0x" << std::hex << s2base;
	std::string addressStr = oss.str();
	std::ostringstream oss2;
	oss2 << "0x" << std::hex << s2baseOff;
	std::string addressStr2 = oss2.str();
	Console::infoPrint("s2_mp64_ship Base at: " + addressStr);
	Console::infoPrint("s2_mp64_ship BaseOff at: " + addressStr2);
}

//0 - CLI, 1 - GUI, 2 - BOTH
void ExtConsole::extConInit(int extConsoleMode) {
	hProcess = GetCurrentProcess();
	if (extConsoleMode >= 1) {
		//wait for external console gui to be fully ready
		while (!ExternalConsoleGui::isExtConGuiReady()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
	if (extConsoleMode == 0 || extConsoleMode == 2) {
		//title stuff
		int size = MultiByteToWideChar(CP_UTF8, 0, "S2MP-Mod External Console", -1, NULL, 0);
		wchar_t* wtitle = new wchar_t[size];
		MultiByteToWideChar(CP_UTF8, 0, "S2MP-Mod External Console", -1, wtitle, size);
		SetConsoleTitle(wtitle);
		std::cout << "extConsoleMode:" << extConsoleMode << "; CLI will be used" << std::endl;
	}

	infoPrintOffsets();
	Console::print("Sys_Cwd(): " + std::string(Functions::_Sys_Cwd()));

	MH_STATUS status = MH_Initialize();
	if (status != MH_OK) {
		std::string sStatus = MH_StatusToString(status);
		Console::print("Minhook init failed");
		//TODO: Add some like error handling here or a global var
	}

	DebugPatches::init();
	//PrintPatches::init();
	DevPatches::init();
	DvarInterface::init();

	if (extConsoleMode == 0 || extConsoleMode == 2) {
		consoleMainLoop();
	}
}