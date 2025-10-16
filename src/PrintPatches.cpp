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
#include "GameUtil.hpp"

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

typedef void(*Online_PatchStreamer_va)(const char* label, const char* fmt, ...);
Online_PatchStreamer_va _Online_PatchStreamer_va = nullptr;

typedef void(*Com_WriteConfig_f)(int localClientNum, const char* name);
Com_WriteConfig_f _Com_WriteConfig_f = nullptr;

typedef char(*DB_FileExists)(const char* zoneName, FF_DIR source);
DB_FileExists _DB_FileExists = nullptr;

typedef char(*LUI_Error)(const char* error, void* luiVm);
LUI_Error _LUI_Error = nullptr;

typedef char(*_printf)(const char* const Format, ...);
_printf __printf = nullptr;

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

void hook_Online_PatchStreamer_va(const char* label, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char cleanFmt[1024];
    size_t j = 0;
    for (size_t i = 0; fmt[i] != '\0' && j < sizeof(cleanFmt) - 1; ++i) {
        if (fmt[i] != '\n' && fmt[i] != '\r') {
            cleanFmt[j++] = fmt[i];
        }
    }
    cleanFmt[j] = '\0';

    std::string cleanerFmt = GameUtil::sanitizeFormatWidths(cleanFmt); //it was looking awful in both consoles

    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), cleanerFmt.c_str(), args);
    Console::printf("%s: %s\n", label, buffer);

    va_end(args);
}

void hook_Com_WriteConfig_f(int localClientNum, const char* name) {
    if (Functions::_Dvar_FindVar("1467")->current.integer) { //fs_debug
        Console::printf("Writing to config file '%s' for local client %d", name, localClientNum);
    }
    
    _Com_WriteConfig_f(localClientNum, name);
}

void hook_DB_FileExists(const char* zoneName, FF_DIR source) {
    if (Functions::_Dvar_FindVar("1467")->current.integer) { //fs_debug
        Console::printf("Checking if file '%s' exists in %s", zoneName, source ? "usermaps directory" : "default directory");
    }
    _DB_FileExists(zoneName, source);
}

void hook_LUI_Error(const char* error, void* luiVm) {
    Console::printf("LUI Error: %s", Functions::_SEH_SafeTranslateString(error));
    _LUI_Error(error, luiVm);
}

void hook_printf(const char* const Format, ...) {
    if (!Format) {
        Console::print("format is NULL");
        return;
    }
    constexpr size_t BUFSZ = 4096;
    char buf[BUFSZ];

    va_list args;
    va_start(args, Format);
    std::vsnprintf(buf, BUFSZ, Format, args);
    va_end(args);

    Console::printf(buf);
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

    //Online_PatchStreamer printing
    MH_CreateHook(reinterpret_cast<void*>(0x2F3F30_b), &hook_Online_PatchStreamer_va, reinterpret_cast<void**>(&_Online_PatchStreamer_va));
    MH_EnableHook(reinterpret_cast<void*>(0x2F3F30_b));

    //fs_debug required
    //Writing to config file '%s' for local client %d
    MH_CreateHook(reinterpret_cast<void*>(0xF7180_b), &hook_Com_WriteConfig_f, reinterpret_cast<void**>(&_Com_WriteConfig_f));
    MH_EnableHook(reinterpret_cast<void*>(0xF7180_b));
    
    //fs_debug required
    //Checking if file '%s' exists in %s
    MH_CreateHook(reinterpret_cast<void*>(0xFA770_b), &hook_DB_FileExists, reinterpret_cast<void**>(&_DB_FileExists));
    MH_EnableHook(reinterpret_cast<void*>(0xFA770_b));
    

    //LUI_Error
    MH_CreateHook(reinterpret_cast<void*>(0x122BD0_b), &hook_LUI_Error, reinterpret_cast<void**>(&_LUI_Error));
    MH_EnableHook(reinterpret_cast<void*>(0x122BD0_b));

    //printf
    //havent found anything that uses this yet
    //MH_CreateHook(reinterpret_cast<void*>(0x31619E_b), &hook_printf, reinterpret_cast<void**>(&__printf));
    //MH_EnableHook(reinterpret_cast<void*>(0x31619E_b));
}
