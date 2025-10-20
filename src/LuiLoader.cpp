#include "pch.h"
#include "Loaders.hpp"
#include <fstream>
#include <filesystem>
#include "FuncPointers.h"

typedef int(*hks_load)(lua_State* state, void* compiler_options, void* reader, void* reader_data, const char* chunk_name);
hks_load _hks_load = nullptr;

void dumpLuaFile(const LuaFile* luaFile) {
    if (!luaFile || !luaFile->name || !luaFile->buffer || luaFile->len <= 0) {
        Console::printf("Invalid LuaFile passed to %s", __FUNCTION__);
        return;
    }
    std::filesystem::path filePath = std::filesystem::path("S2MP-Mod") / "dump" / luaFile->name;
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
        if (Functions::_Dvar_FindVar("g_dumpLui")->current.enabled) {
            dumpLuaFile(*luaFile);
        }
        // Console::devPrint("Loaded LUA File: " + std::string((*luaFile)->name));
        //
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



void hook_hks_load(lua_State* state, void* compiler_options, void* reader, void* reader_data, const char* chunk_name) {
    //HksBytecodeSharingMode origShare = state->m_global->m_bytecodeSharingMode;
    //state->m_global->m_bytecodeSharingMode = HKS_BYTECODE_SHARING_ON;



    _hks_load(state, compiler_options, reader, reader_data, chunk_name);
}

void LuiLoader::init() {
    Hook_Load_LuaFileAsset();

    //hks_load
    MH_CreateHook(reinterpret_cast<void*>(0x338D90_b), &hook_hks_load, reinterpret_cast<void**>(&_hks_load));
    MH_EnableHook(reinterpret_cast<void*>(0x338D90_b));
}