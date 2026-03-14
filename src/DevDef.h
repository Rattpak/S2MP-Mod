#pragma once
#include <string>
#include "structs.h"
#include "FuncPointers.h"
#include <vector>



///////////////////////////////////////
#define DEVELOPMENT_BUILD
//#define ARXAN_DEBUG_INFO
//#define ARXAN_LOG_VEH

///////////////////////////////////////

//muh macro
#ifdef DEVELOPMENT_BUILD
#define DEV_INIT_PRINT() Console::initPrint(std::string(__FUNCTION__))
#else
#define DEV_INIT_PRINT() ((void)0)
#endif

#ifdef DEVELOPMENT_BUILD
#define DEV_PRINTF(fmt, ...) \
    do { \
        Console::printf("[DEV] " fmt, ##__VA_ARGS__); \
    } while (0)
#else
#define DEV_PRINTF(fmt, ...) \
    do { } while (0)
#endif

#ifdef DEVELOPMENT_BUILD
#define DEV_ONLY_FUNCTION() ((void)0)
#else
#define DEV_ONLY_FUNCTION() static_assert(false, \
        "This function is only available in DEVELOPMENT_BUILD")
#endif

class DevPatches {
public:
#ifdef DEVELOPMENT_BUILD
	static bool customLocStr(const char* name, XAssetHeader& header);
	static void doEulaTest();
#endif

	static void imgtest();

	static void init();
};

class DevDraw {
public:
	static void toggleEntityDebugGui();
	static void renderIntConDebugGui(int windowWidth, int windowHeight);
	static void toggleIntConDebugGui();
	static void render(int windowWidth, int windowHeight);
	static std::string getDevBuildDate();
	static void renderDevGui(std::vector<std::string>& list, int xPos, int yPos, int wWid, int wHei, float* color, font_t* font);
	static void toggleLuaDebugGui();
	static void toggleAntiCheatDebugGui();

	static material_t* previewMaterial;
};