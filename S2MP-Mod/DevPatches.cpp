/////////////////////////////////////////////////
//         Development Patches
// Temporary patches for testing stuff
/////////////////////////////////////////////////

#include "pch.h"
#include "DevPatches.hpp"
#include "Console.hpp"
#include <MinHook.h>
#include "FuncPointers.h"

uintptr_t DevPatches::base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;

void DevPatches::init() {
    Console::initPrint("DevPatches::init()");
}