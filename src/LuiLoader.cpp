#include "pch.h"
#include "Loaders.hpp"
#include <fstream>
#include <filesystem>
#include "FuncPointers.h"
#include "DevDef.h"

typedef int(*hks_load)(void* state, void* compiler_options, void* reader, void* reader_data, const char* chunk_name);
hks_load _hks_load = nullptr;

typedef void(*LUI_CoD_Init)(bool frontend);
LUI_CoD_Init _LUI_CoD_Init = nullptr;

typedef void(*LUI_CoD_Shutdown)();
LUI_CoD_Shutdown _LUI_CoD_Shutdown = nullptr;

static const DWORD OFFSET_GLOBAL_FROM_STATE = 0x10;
static const DWORD OFFSET_BYTECODE_SHARING_FROM_GLOBAL = 0x1C0;
static const int   BYTECODE_SHARING_ON = 1;

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
    else {
        DEV_PRINTF("!(luaFile && *luaFile) ", __FUNCTION__);
    }

    oLoad_LuaFileAsset(luaFile);
}

void Hook_Load_LuaFileAsset() {
    void* target = (void*)(0xD81A0_b);

    if (MH_CreateHook(target, &Load_LuaFileAsset_hookfunc, reinterpret_cast<void**>(&oLoad_LuaFileAsset)) != MH_OK) {
        DEV_PRINTF("CREATEHOOK FAILURE IN FUNCTION ", __FUNCTION__);
        return;
    }

    if (MH_EnableHook(target) != MH_OK) {
        DEV_PRINTF("ENABLEHOOK FAILURE IN FUNCTION ", __FUNCTION__);
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

inline int* getBytecodeSharingPtr(void* state) {
    if (OFFSET_BYTECODE_SHARING_FROM_GLOBAL == 0) {
        return nullptr;
    }
    char* base;
    if (OFFSET_GLOBAL_FROM_STATE != 0) {
        void* global = *(void**)((char*)state + OFFSET_GLOBAL_FROM_STATE);
        if (!global) {
            return nullptr;
        }
        base = (char*)global;
    }
    else {
        base = (char*)state;
    }
    return (int*)(base + OFFSET_BYTECODE_SHARING_FROM_GLOBAL);
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

int hook_hks_load(void* state, void* compilerOptions, void* reader, void* readerData, const char* chunkName) {


    return _hks_load(state, compilerOptions, reader, readerData, chunkName);
}

void hook_LUI_CoD_Init(bool frontend) {
    _LUI_CoD_Init(frontend);

}

void hook_LUI_CoD_Shutdown() {
    Console::printf("LUI: Shutting Down");
    _LUI_CoD_Shutdown();
}


//return true if XAssetHeader was loaded custom
//false otherwise
bool LuiLoader::LUI_CoD_GetRawFile(XAssetHeader& header, const char* name) {

    return false;
}

void LuiLoader::init() {
    Hook_Load_LuaFileAsset(); //for loadtime dump

    //hks_load
    MH_CreateHook(reinterpret_cast<void*>(0x2D6D10_b), &hook_hks_load, reinterpret_cast<void**>(&_hks_load));
    MH_EnableHook(reinterpret_cast<void*>(0x2D6D10_b));

    //LUI_CoD_Init
    MH_CreateHook(reinterpret_cast<void*>(0x317A00_b), &hook_LUI_CoD_Init, reinterpret_cast<void**>(&_LUI_CoD_Init));
    MH_EnableHook(reinterpret_cast<void*>(0x317A00_b));

    //LUI_CoD_Shutdown
    MH_CreateHook(reinterpret_cast<void*>(0x31A6F0_b), &hook_LUI_CoD_Shutdown, reinterpret_cast<void**>(&_LUI_CoD_Shutdown));
    MH_EnableHook(reinterpret_cast<void*>(0x31A6F0_b));



}