#include "pch.h"
#include "Loaders.hpp"
#include <fstream>
#include <filesystem>

void dumpLuaFile(const LuaFile* luaFile) {
    if (!luaFile || !luaFile->name || !luaFile->buffer || luaFile->len <= 0) {
        Console::devPrint("Invalid LuaFile passed to DumpLuaFile");
        return;
    }
    std::filesystem::path filePath = std::filesystem::path("dump") / luaFile->name;
    std::filesystem::create_directories(filePath.parent_path());

    try {
        std::ofstream out(filePath, std::ios::binary);
        if (!out) {
            Console::devPrint("Failed to open file for writing: " + filePath.string());
            return;
        }

        out.write(reinterpret_cast<const char*>(luaFile->buffer), luaFile->len);
        out.close();

        Console::devPrint("Dumped LuaFile to: " + filePath.string());
    }
    catch (const std::exception& e) {
        Console::devPrint("Exception during LuaFile dump: " + std::string(e.what()));
    }
}

typedef void(__fastcall* Load_LuaFileAsset_t)(LuaFile** luaFile);
static Load_LuaFileAsset_t oLoad_LuaFileAsset = nullptr;

void __fastcall Load_LuaFileAsset_hookfunc(LuaFile** luaFile) {
    if (luaFile && *luaFile) {
        // Console::devPrint("Loaded LUA File: " + std::string((*luaFile)->name));
        //dumpLuaFile(*luaFile);
    }

    oLoad_LuaFileAsset(luaFile);
}

void Hook_Load_LuaFileAsset() {
    void* target = (void*)(0x13BB70_b);

    if (MH_CreateHook(target, &Load_LuaFileAsset_hookfunc, reinterpret_cast<void**>(&oLoad_LuaFileAsset)) != MH_OK) {
        Console::devPrint("ERROR: MH_CreateHook failure in function " + std::string(__FUNCTION__));
        return;
    }

    if (MH_EnableHook(target) != MH_OK) {
        Console::devPrint("ERROR: MH_EnableHook failure in function " + std::string(__FUNCTION__));
        return;
    }
}

void LuiLoader::init() {
    Hook_Load_LuaFileAsset();
}