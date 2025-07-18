#include "pch.h"
#include "client/game/structs.h"
#include "Console.hpp"
#include "FuncPointers.h"
#include <MinHook.h>
#include "GameUtil.hpp"
#include "DevDef.h"

typedef void (*R_EndFrame_t)(void);
R_EndFrame_t _EndFrame = nullptr;

float colorWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };


void R_EndFrame_hookfunc() {
   int winWidth = *(int*)(base + 0x1DA11E8); //this isnt actually window size i think its bink player size
   int winHeight = *(int*)(base + 0x1DA11EC);

    DevDraw::render(winWidth,  winHeight);

    _EndFrame();
}

void renderHookInit() {
    void* target = (void*)(base + 0x92E430); //endframe
    MH_Initialize();
    MH_CreateHook(target, &R_EndFrame_hookfunc, reinterpret_cast<void**>(&_EndFrame));
    MH_EnableHook(target);
}


void InternalConsole::init() {
    renderHookInit();

}