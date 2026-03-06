#include "pch.h"
#include "Loaders.hpp"
#include <fstream>
#include <filesystem>
#include "FuncPointers.h"
#include "DevDef.h"

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
            DEV_PRINTF("Failed to open file for writing: %s", filePath.string().c_str());
            return;
        }

        out.write(reinterpret_cast<const char*>(luaFile->buffer), luaFile->len);
        out.close();

        DEV_PRINTF("Dumped LuaFile to: %s", filePath.string().c_str());
    }
    catch (const std::exception& e) {
        DEV_PRINTF("Exception during LuaFile dump: %s",e.what());
    }
}

typedef void(__fastcall* Load_LuaFileAsset_t)(LuaFile** luaFile);
static Load_LuaFileAsset_t oLoad_LuaFileAsset = nullptr;

void __fastcall Load_LuaFileAsset_hookfunc(LuaFile** luaFile) {
    if (luaFile && *luaFile) {
        if (Functions::_Dvar_FindVar("g_dumpLui")->current.enabled) {
            dumpLuaFile(*luaFile);
        }
    }

    oLoad_LuaFileAsset(luaFile);
}

void Hook_Load_LuaFileAsset() {
    void* target = (void*)(0xD81A0_b);

    if (MH_CreateHook(target, &Load_LuaFileAsset_hookfunc, reinterpret_cast<void**>(&oLoad_LuaFileAsset)) != MH_OK) {
        return;
    }

    if (MH_EnableHook(target) != MH_OK) {
        return;
    }
}

//////////////////////////////
//                          //
//  Lui Loader starts here  //
//                          //
//////////////////////////////

struct script {
    std::string name;
    std::string root;
};

std::string requireCaller;
std::string rawLuaFileName;
std::vector<script> loadedLua;
bool doCustomLoad = false;

bool isScriptLoaded(const std::string& name) {
    for (const script& lua : loadedLua) {
        if (lua.name == name) {
            return true;
        }
    }

    return false;
}

//called from actual DB_FindXAssetHeader
//return true if we loaded a custom lua file
bool LuiLoader::FindXAssetHeader(XAssetType type, const char* name, int allow_create_default, XAssetHeader &header) {
    if (isScriptLoaded(requireCaller)) {

    }
    return false;
}

int loadBuffer(const std::string& name, const std::string& data) {
    lua_State* luastate = (lua_State*)0x1CEA890_b;
    HksBytecodeSharingMode origShare = luastate->m_global->m_bytecodeSharingMode;
    luastate->m_global->m_bytecodeSharingMode = HKS_BYTECODE_SHARING_ON;
    HksCompilerSettings settings{};
    int ret = Functions::_hksi_hksL_loadbuffer(luastate, &settings, data.data(), data.size(), name.data());
    luastate->m_global->m_bytecodeSharingMode = origShare;
    return ret;
}

int hook_hks_load(lua_State* state, void* compiler_options, void* reader, void* reader_data, const char* chunk_name) {
   
    //if (doCustomLoad) {
    //    doCustomLoad = false;
    //    loadedLua.push_back({ rawLuaFileName, requireCaller });
    //    return loadBuffer(rawLuaFileName, )
    //}


    return _hks_load(state, compiler_options, reader, reader_data, chunk_name);
}

void LuiLoader::init() {
    Hook_Load_LuaFileAsset();

    //hks_load
    MH_CreateHook(reinterpret_cast<void*>(0x2D6D10_b), &hook_hks_load, reinterpret_cast<void**>(&_hks_load));
    MH_EnableHook(reinterpret_cast<void*>(0x2D6D10_b));
}