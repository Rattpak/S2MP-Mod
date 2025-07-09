#include "pch.h"
#include "structs.h"
#include "Console.hpp"
#include "FuncPointers.h"
#include <MinHook.h>
#include "GameUtil.hpp"
#include "DevDef.h"

std::string devBuildDate = "DEV BUILD: " + std::string(__DATE__) + " " + std::string(__TIME__);

//add USE_BUILD_USER to your preprocessor definitions to use this feature
//create a "build_user.txt" with the rest of the .cpp/.h files with #define BUILD_USER "username here" inside
std::string compiledBy = "COMPILED BY: Andrew";

typedef void (*R_EndFrame_t)(void);
R_EndFrame_t _EndFrame = nullptr;

float colorWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float watermarkCol[4] = { 1.0f, 1.0f, 1.0f, 0.35f };
float devBuildInfoColor[4] = { 1.0f, 0.0f, 0.0f, 0.55f };


void drawDevelopmentInfo(int windowW, int windowH) {
    font_t* font = Functions::_R_RegisterFont("fonts/fira_mono_regular.ttf", 16);
    font_t* devFont = Functions::_R_RegisterFont("fonts/fira_mono_regular.ttf", 16);
    //font_t* devFont = Functions::_R_RegisterFont("fonts/consoleFont", 10);

    if (font) {
        Functions::_R_AddCmdDrawText("S2MP-MOD PROTOTYPE BUILD (STEAM)", 0x7fffffff, font, 0, 0, font->pixelHeight, 2, 1 + font->pixelHeight, 1.0f, 1.0f, 0.0f, watermarkCol, 0);

        if (devFont) {
            std::string devBuildInfoText = devBuildDate;
#ifdef USE_BUILD_USER
            devBuildInfoText += " " + compiledBy;
#endif
            Functions::_R_AddCmdDrawText(devBuildInfoText.c_str(), 0x7fffffff, devFont, 0, 0, font->pixelHeight, 2, windowH - 2, 1.0f, 1.0f, 0.0f, devBuildInfoColor, 0);
        }
    }
}

void R_EndFrame_hookfunc() {
    
   int winWidth = *(int*)(GameUtil::base + 0x1DA11E8); //this isnt actually window size i think its bink player size
   int winHeight = *(int*)(GameUtil::base + 0x1DA11EC);

#ifdef DEVELOPMENT_BUILD
    drawDevelopmentInfo(winWidth, winHeight);
#endif // DEVELOPMENT_BUILD

    _EndFrame();
}

void renderHookInit() {
    void* target = (void*)(GameUtil::base + 0x92E430); //endframe
    MH_Initialize();
    MH_CreateHook(target, &R_EndFrame_hookfunc, reinterpret_cast<void**>(&_EndFrame));
    MH_EnableHook(target);
}

void InternalConsole::init() {
    renderHookInit();
}