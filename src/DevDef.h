#pragma once
#include <string>
#include "client/game/structs.h"
#include "FuncPointers.h"
#include <vector>

///////////////////////////////////////
#define DEVELOPMENT_BUILD
#define ARXAN_DEBUG_INFO
//#define ARXAN_LOG_VEH

///////////////////////////////////////



class DevPatches {
public:
	static void init();
};

class DevDraw {
public:
	static void toggleEntityDebugGui();
	static void render(int windowWidth, int windowHeight);
	static void renderDevGui(std::vector<std::string>& list, int xPos, int yPos, int wWid, int wHei, float* color, font_t* font);
	static void toggleLuaDebugGui();
};