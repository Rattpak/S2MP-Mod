///////////////////////////////////////////////////////
//                  Print Patches
//	For reimplementing stripped print statements
////////////////////////////////////////////////////////
#include "pch.h"
#include "Console.hpp"
#include "PrintPatches.hpp"
#include <MinHook.h>
#include <FuncPointers.h>
#include "structs.h"

typedef void(*CM_LoadMap)(const char* name, int* checksum);
CM_LoadMap _CM_LoadMap = nullptr;

typedef void(*FS_Startup)(const char* gameName);
FS_Startup _FS_Startup = nullptr;

typedef void(*SV_Shutdown)(const char* finalmsg);
SV_Shutdown _SV_Shutdown = nullptr;

typedef void(*SV_SpawnServer)(const char* server, int mapIsPreloaded, int savegame, int isRestart);
SV_SpawnServer _SV_SpawnServer = nullptr;

typedef void(*LUI_LoadLuaFile)(const char* filename, void* luivm);
LUI_LoadLuaFile _LUI_LoadLuaFile = nullptr;

typedef void(*LUI_Init)(void* func, void* alsofunc);
LUI_Init _LUI_Init = nullptr;

typedef void(*DB_TryLoadXFileInternal)(const char* zoneName, int zoneFlags, int isBaseMap);
DB_TryLoadXFileInternal _DB_TryLoadXFileInternal = nullptr;

typedef void(*DB_LoadXZone)(XZoneInfo* zoneInfo, __int64 zoneCount, __int64 waitAlloc, __int64 skipReadAlwaysLoadedAssets);
DB_LoadXZone _DB_LoadXZone = nullptr;

typedef void(*G_InitGame)(int levelTime, unsigned int randomSeed, int restart, int registerDvars, int savegame);
G_InitGame _G_InitGame = nullptr;

void hook_CM_LoadMap(const char* name, int* checksum) {
    if (name) {
        Console::printf("Loading Map: %s", name);
    }
    else {
        Console::infoPrint("Invalid name passed into CM_LoadMap");
    }
    _CM_LoadMap(name, checksum);
}

void hook_FS_Startup(const char* gameName) {
    Console::print("----- FS_Startup -----");
    _FS_Startup(gameName);
}

void hook_SV_Shutdown(const char* finalmsg) {
    Console::print("----- Server Shutdown -----");
    _SV_Shutdown(finalmsg);
}

void hook_SV_SpawnServer(const char* server, int mapIsPreloaded, int savegame, int isRestart) {
    Console::print("------ Server Initialization ------");
    Console::printf("Server: %s", server);
    
    _SV_SpawnServer(server, mapIsPreloaded, savegame, isRestart);
}

void hook_LUI_LoadLuaFile(const char* filename, void* luivm) {
    Console::printf("LUI: Loading LUA File \"%s\"", filename);
    _LUI_LoadLuaFile(filename, luivm);
}

void hook_LUI_Init(void* func, void* alsofunc) {
    Console::print("LUI: Starting up...");
    _LUI_Init(func, alsofunc);
}

void hook_DB_TryLoadXFileInternal(const char* zoneName, int zoneFlags, int isBaseMap) {
    Console::printf("Loading Zone: %s.ff", zoneName);
    _DB_TryLoadXFileInternal(zoneName, zoneFlags, isBaseMap);
}

void hook_DB_LoadXZone(XZoneInfo* zoneInfo, __int64 zoneCount, __int64 waitAlloc, __int64 skipReadAlwaysLoadedAssets) {
    Console::printf("Adding fastfile '%s' to queue", zoneInfo->name);
    _DB_LoadXZone(zoneInfo, zoneCount, waitAlloc, skipReadAlwaysLoadedAssets);
}

void hook_G_InitGame(int levelTime, unsigned int randomSeed, int restart, int registerDvars, int savegame) {
    Console::print("------- Game Initialization -------");
    Console::print("gamename: S2");
    Console::print("gamedate: Nov 24 2019");
    _G_InitGame(levelTime, randomSeed, restart, registerDvars, savegame);
}

void PrintPatches::init() {
	Console::infoPrint(__FUNCTION__);

    //Loading Map:
    MH_CreateHook(reinterpret_cast<void*>(0x6A1050_b), &hook_CM_LoadMap, reinterpret_cast<void**>(&_CM_LoadMap));
    MH_EnableHook(reinterpret_cast<void*>(0x6A1050_b));

    //----- FS_Startup -----
    MH_CreateHook(reinterpret_cast<void*>(0x7BE630_b), &hook_FS_Startup, reinterpret_cast<void**>(&_FS_Startup));
    MH_EnableHook(reinterpret_cast<void*>(0x7BE630_b));

    //----- Server Shutdown -----
    MH_CreateHook(reinterpret_cast<void*>(0x743F30_b), &hook_SV_Shutdown, reinterpret_cast<void**>(&_SV_Shutdown));
    MH_EnableHook(reinterpret_cast<void*>(0x743F30_b));

    //------ Server Initialization ------
    MH_CreateHook(reinterpret_cast<void*>(0x744070_b), &hook_SV_SpawnServer, reinterpret_cast<void**>(&_SV_SpawnServer));
    MH_EnableHook(reinterpret_cast<void*>(0x744070_b));

    //LUI: Loading LUA File \"%s\""
    MH_CreateHook(reinterpret_cast<void*>(0x129D30_b), &hook_LUI_LoadLuaFile, reinterpret_cast<void**>(&_LUI_LoadLuaFile));
    MH_EnableHook(reinterpret_cast<void*>(0x129D30_b));

    //LUI: Starting up...
    MH_CreateHook(reinterpret_cast<void*>(0x1282F0_b), &hook_LUI_Init, reinterpret_cast<void**>(&_LUI_Init));
    MH_EnableHook(reinterpret_cast<void*>(0x1282F0_b));

    //Loading Zone: 
    MH_CreateHook(reinterpret_cast<void*>(0x105DA0_b), &hook_DB_TryLoadXFileInternal, reinterpret_cast<void**>(&_DB_TryLoadXFileInternal));
    MH_EnableHook(reinterpret_cast<void*>(0x105DA0_b));

    //Adding fastfile '%s' to queue
    MH_CreateHook(reinterpret_cast<void*>(0xFF510_b), &hook_DB_LoadXZone, reinterpret_cast<void**>(&_DB_LoadXZone));
    MH_EnableHook(reinterpret_cast<void*>(0xFF510_b));

    //------- Game Initialization -------
    MH_CreateHook(reinterpret_cast<void*>(0x5C3C60_b), &hook_G_InitGame, reinterpret_cast<void**>(&_G_InitGame));
    MH_EnableHook(reinterpret_cast<void*>(0x5C3C60_b));




}
