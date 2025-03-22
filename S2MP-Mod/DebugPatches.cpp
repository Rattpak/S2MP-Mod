///////////////////////////////////////////
//         Debug Patches
//	Probably not needed anymore
////////////////////////////////////////////
#include "pch.h"
#include "Console.hpp"
#include "DebugPatches.hpp"
#include "GameUtil.hpp"
#include <MinHook.h>
#include <Windows.h>
#include <array>
#include <mutex>

std::string DebugPatches::conLabel = "DP";
uintptr_t DebugPatches::base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


void DebugPatches::init() {
	Console::initPrint("DebugPatches::init()");
}
