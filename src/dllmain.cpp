#include "pch.h"
#include "Console.hpp"
#include "client/game/structs.h"
#include <Hook.hpp>

HMODULE hm;
//0 - CLI, 1 - GUI, 2 - BOTH
const int EXTERNAL_CONSOLE_MODE = 1;

DWORD WINAPI extConInitWrapper(LPVOID) {
	ExtConsole::extConInit(EXTERNAL_CONSOLE_MODE);
	return NULL;
}

void ExternalConsoleGuiInitWrapper() {
	ExternalConsoleGui::init(hm, 0, NULL, 1);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved) {
	hm = hModule;
	if (dwReason == DLL_PROCESS_ATTACH) 
	{
		utils::hook::detour::detour(); // initialize minhook

		if (EXTERNAL_CONSOLE_MODE == 0 || EXTERNAL_CONSOLE_MODE == 2) {
			AllocConsole();
			FILE* pFile = nullptr;
			freopen_s(&pFile, "CONOUT$", "w", stdout);
			freopen_s(&pFile, "CONIN$", "r", stdin);
		}
		if (EXTERNAL_CONSOLE_MODE >= 1) {
			std::thread t1(ExternalConsoleGuiInitWrapper);
			t1.detach();
		}
		CreateThread(0, 0, extConInitWrapper, 0, 0, 0);
	}
	return TRUE;
}