#include "pch.h"
#include "DevDef.h"


std::string devBuildDate = "DEV BUILD: " + std::string(__DATE__) + " " + std::string(__TIME__);

#ifdef USE_BUILD_USER
//add USE_BUILD_USER to your preprocessor definitions to use this feature
//create a "build_user.txt" with the rest of the .cpp/.h files with #define BUILD_USER "username here" inside
#include "build_user.txt"
std::string compiledBy = "COMPILED BY: " + std::string(BUILD_USER);
#endif

#include <MinHook.h>
#include <GameUtil.hpp>
#include <Console.hpp>

float watermarkCol[4] = { 1.0f, 1.0f, 1.0f, 0.35f };
float devBuildInfoColor[4] = { 1.0f, 0.0f, 0.0f, 0.55f };
float luiDebugGuiColor[4] = {0.54f, 0.32f, 0.2f, 1.0f };
float entDebugGuiColor[4] = { 0.27f, 0.75f, 0.27f, 1.0f };
float devCrosshairColor[4] = {1.0f, 1.0f, 1.0f, 0.8f };
float debuggerBgColor[4] = {0.0f, 0.0f, 0.0f, 0.7f };
float intConDbgColor[4] = { 1.0f, 0.5f, 0.0f, 1.0f };

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

void DevDraw::renderDevGui(std::vector<std::string>& list, int xPos, int yPos, int wWid, int wHei, float* color, font_t* font) {
    int bgOffset = 3;
    int bgTempWidth = 275;
    int bgRowSize = 15;
    Functions::_R_AddCmdDrawStretchPic(xPos - bgOffset, yPos - bgOffset - bgRowSize, bgTempWidth + bgOffset, bgRowSize * list.size() + bgRowSize, 0.0f, 0.0f, 0.0f, 0.0f, debuggerBgColor, InternalConsole::getMaterialWhite());
    for (int i = 0; i < list.size(); i++) {
        Functions::_R_AddCmdDrawText(list[i].c_str(), 0x7fffffff, font, 0, 0, font->pixelHeight, xPos, yPos + (i*font->pixelHeight) + 2, 1.0f, 1.0f, 0.0f, color, 0);
    }
}

bool debugLuaGui = false;
int luaMemHighWatermark = 0;
void renderLuaDebugGui(int windowWidth, int windowHeight) {
    if (!debugLuaGui) {
        return;
    }
    std::vector<std::string> guiTextList;
    font_t* conFont = Functions::_R_RegisterFont("fonts/consoleFont", 15);

    int max = Functions::_LUI_CoD_GetMaxMemory();
    if (max <= 0) {
        guiTextList.push_back("S2MP-MOD LUA DEBUGGER [NOT READY]");
        DevDraw::renderDevGui(guiTextList, 15, 35, windowWidth, windowHeight, luiDebugGuiColor, conFont);
        return;
    }

    int used = (max - (unsigned int)Functions::_LUI_CoD_GetFreeMemoryBytes()) >> 10;
    if (used > luaMemHighWatermark) {
        luaMemHighWatermark = used;
    }
    int g_lui_lock_level = *(int*)(0x1C675F4_b);
    bool g_virtualLobbyActive = *(bool*)(0x1C675F2_b);
    guiTextList.push_back("S2MP-MOD LUA DEBUGGER");
    guiTextList.push_back("Lua memory used: " + std::to_string(used) + " / " + std::to_string(max / 1024) + " KB");
    guiTextList.push_back("Lua memory high watermark: " + std::to_string(luaMemHighWatermark) + " KB");
    //guiTextList.push_back("Lua lock level: " + std::to_string(g_lui_lock_level));
    guiTextList.push_back("Virtual lobby active: " + std::string(g_virtualLobbyActive ? "true" : "false"));

    DevDraw::renderDevGui(guiTextList, 15, 35, windowWidth, windowHeight, luiDebugGuiColor, conFont);
}

void DevDraw::toggleLuaDebugGui() {
    debugLuaGui = !debugLuaGui;
}

bool debugEntGui = false;
int entityHighWatermark = 0;
void renderEntDebugGui(int windowWidth, int windowHeight) {
    if (!debugEntGui) {
        return;
    }
    std::vector<std::string> guiTextList;
    font_t* conFont = Functions::_R_RegisterFont("fonts/consoleFont", 15);

    int entCount = *(int*)(0xA2D6DD0_b);

    if (entCount <= 0) {
        guiTextList.push_back("S2MP-MOD ENT DEBUGGER [NOT READY]");
        DevDraw::renderDevGui(guiTextList, 315, 35, windowWidth, windowHeight, entDebugGuiColor, conFont);
        return;
    }

    if (entCount > entityHighWatermark) {
        entityHighWatermark = entCount;
    }
    guiTextList.push_back("S2MP-MOD ENT DEBUGGER");
    guiTextList.push_back("G_Spawn Entities used: " + std::to_string(entCount) + " / 2046");
    guiTextList.push_back("G_Spawn Ent high watermark: " + std::to_string(entityHighWatermark));

    DevDraw::renderDevGui(guiTextList, 315, 35, windowWidth, windowHeight, entDebugGuiColor, conFont);
}
void DevDraw::toggleEntityDebugGui() {
    debugEntGui = !debugEntGui;
}

bool debugAntiCheatGui = false;
void rednerAntiCheatDebugGui(int windowWidth, int windowHeight) {
    if (!debugAntiCheatGui) {
        return;
    }
    std::vector<std::string> guiTextList;
    font_t* conFont = Functions::_R_RegisterFont("fonts/consoleFont", 15);

    int s_playerInfractionsNum = *(int*)(0xE463D98_b);

    guiTextList.push_back("S2MP-MOD AC DEBUGGER");
    guiTextList.push_back("s_playerInfractionsNum: " + std::to_string(s_playerInfractionsNum));

    DevDraw::renderDevGui(guiTextList, 615, 35, windowWidth, windowHeight, entDebugGuiColor, conFont);
}

void DevDraw::toggleAntiCheatDebugGui() {
    debugAntiCheatGui = !debugAntiCheatGui;
}

bool drawIntConDbg = false;
void DevDraw::renderIntConDebugGui(int windowWidth, int windowHeight) {
    if (!drawIntConDbg) {
        return;
    }
    std::vector<std::string> guiTextList;
    font_t* conFont = Functions::_R_RegisterFont("fonts/consoleFont", 15);

    guiTextList.push_back("S2MP-MOD INTERNAL CONSOLE DEBUGGER");
    guiTextList.push_back("isShift: " + std::string(InternalConsole::DEVONLY_isShift() ? "true" : "false"));
    guiTextList.push_back("isAlt: " + std::string(InternalConsole::DEVONLY_isAlt() ? "true" : "false"));
    guiTextList.push_back("isCtrl: " + std::string(InternalConsole::DEVONLY_isCtrl() ? "true" : "false"));
    guiTextList.push_back("fullConsole: " + std::string(InternalConsole::DEVONLY_fullConsole() ? "true" : "false"));
    guiTextList.push_back("consoleOpen: " + std::string(InternalConsole::DEVONLY_consoleOpen() ? "true" : "false"));
    guiTextList.push_back("outputStackSeekPos: " + std::to_string(InternalConsole::DEVONLY_outputStackSeekPos()));
    guiTextList.push_back("maxLines at current res: " + std::to_string(InternalConsole::DEVONLY_maxLines()));
    guiTextList.push_back("scrollbarTrackHeight: " + std::to_string(InternalConsole::DEVONLY_scrollbarTrackHeight()));
    guiTextList.push_back("scrollbarBaseX: " + std::to_string(InternalConsole::DEVONLY_scrollbarBaseX()));
    guiTextList.push_back("sliderHeight: " + std::to_string(InternalConsole::DEVONLY_sliderHeight()));
    guiTextList.push_back("outputStackSize: " + std::to_string(InternalConsole::DEVONLY_outputStackSize()));
    guiTextList.push_back("sliderOffsetY: " + std::to_string(InternalConsole::DEVONLY_sliderOffsetY()));
    guiTextList.push_back("cmdStackSize: " + std::to_string(InternalConsole::DEVONLY_cmdStackSize()));
    guiTextList.push_back("cmdStackSeekPos: " + std::to_string(InternalConsole::DEVONLY_cmdStackSeekPos()));
    guiTextList.push_back("recentKeynum: " + std::to_string(InternalConsole::DEVONLY_recentKeynum()));

    DevDraw::renderDevGui(guiTextList, 1575, 100, windowWidth, windowHeight, intConDbgColor, conFont);
}

void DevDraw::toggleIntConDebugGui() {
    drawIntConDbg = !drawIntConDbg;
}

/*
    Function runs at end of every frame.
    Not everything in here needs to be for DEVELOPMENT_BUILD
*/
void DevDraw::render(int windowWidth, int windowHeight) {
#ifdef DEVELOPMENT_BUILD
    drawDevelopmentInfo(windowWidth, windowHeight);
#endif // DEVELOPMENT_BUILD

    renderLuaDebugGui(windowWidth, windowHeight);
    renderEntDebugGui(windowWidth, windowHeight);
    rednerAntiCheatDebugGui(windowWidth, windowHeight);
}

