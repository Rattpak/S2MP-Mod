#include "pch.h"
#include "SetScore.hpp"
#include "Console.hpp"
#include "Hook.hpp"
#include "FuncPointers.h"

uintptr_t SetScore::base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;
// IN DEV - XxSolarXx


void SetScore::init() {
    Console::infoPrint("SetScore::init()");
}

void SetScore::set(int playerID, int score) {
    if (playerID < 0 || score < 0) {
        Console::print("Invalid player ID or score.");
        return;
    }

    Functions::_SV_SendServerCommand(0i64, 0, "%c \"Setting player %d score to: %d\"", 101i64, playerID, score);

    uintptr_t playerBase = base + (playerID * 0x1F8);
    uintptr_t scoreOffset = playerBase + 0x100;

    HANDLE pHandle = GetCurrentProcess();
    WriteProcessMemory(pHandle, (LPVOID)(scoreOffset), &score, sizeof(int), nullptr);

    Console::print("SetScore executed successfully.");
}
