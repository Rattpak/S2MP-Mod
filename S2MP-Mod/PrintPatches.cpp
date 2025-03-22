///////////////////////////////////////////////////////
//                  Print Patches
//	For reimplementing stripped print statements
////////////////////////////////////////////////////////
#include "pch.h"
#include "Console.hpp"
#include "PrintPatches.hpp"
#include <MinHook.h>

uintptr_t PrintPatches::base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;

typedef void(*CM_LoadMap)(const char* name, int* checksum);
CM_LoadMap _CM_LoadMap = nullptr;

void hook_CM_LoadMap(const char* name, int* checksum) {
    Console::infoPrint("Loading Map: " + std::string(name));
    _CM_LoadMap(name, checksum);
}

void PrintPatches::init() {
	Console::infoPrint("PrintPatches::init()");
	MH_CreateHook(reinterpret_cast<void*>(PrintPatches::base + 0x6A1050), &hook_CM_LoadMap, reinterpret_cast<void**>(&_CM_LoadMap));
    MH_EnableHook(reinterpret_cast<void*>(PrintPatches::base + 0x6A1050));
}
