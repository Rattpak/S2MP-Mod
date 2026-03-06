#include "pch.h"
#include "Loaders.hpp"
#include "FuncPointers.h"
#include <unordered_set>
#include "DevDef.h"

//dont load from here just dump from here
//also has the waited for xasset print so ya

XAssetHeader hook_DB_FindXAssetHeader(XAssetType type, const char* name, int allow_create_default) {
    
    
#ifdef DEVELOPMENT_BUILD
    static std::unordered_set<XAssetType> seenTypes;
    if (seenTypes.find(type) == seenTypes.end()) {
        seenTypes.insert(type);
        Console::printf("New XAssetType: %d (first seen with name: %s)", type, name);
    }
#endif // DEVELOPMENT_BUILD
    XAssetHeader header;

    switch (type) {
    case ASSET_TYPE_LUA_FILE: {
        if (LuiLoader::FindXAssetHeader(type, name, allow_create_default, header)) {
            return header;
        }
        break;
    }
#ifdef DEVELOPMENT_BUILD
    case ASSET_TYPE_LOCALIZE_ENTRY: {
        if (DevPatches::customLocStr(name, header)) {
            return header;
        }
        break;
    }
#endif
    default: {
        break;
    }
    }


    auto start = std::chrono::high_resolution_clock::now();
    const char* safeName = (name && name[0]) ? name : "<null>";
    header = Functions::_DB_FindXAssetHeader(type, name, allow_create_default);
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (dur > 100) {
        Console::printf("Waited %i msec for asset '%s'", dur, safeName);
    }

    switch (type) {
    case ASSET_TYPE_STRINGTABLE: {
        //always dump before load
        if (Functions::_Dvar_FindVar("g_dumpStringTables")->current.enabled) {
            StringTableLoader::dump(header.table);
        }
        break;
    }
    case ASSET_TYPE_RAWFILE: {
        if (Functions::_Dvar_FindVar("g_dumpRawfiles")->current.enabled) {
            RawFileLoader::dump(header.rawfile);
        }
        break;
    }
    default: { 
        break;
    }
    }

    return header;
}


void Loaders::initAssetLoaders() {
    DEV_INIT_PRINT();
	//DB_FindXAssetHeader hook
	MH_CreateHook(reinterpret_cast<void*>(0xFAB20_b), &hook_DB_FindXAssetHeader, reinterpret_cast<void**>(&Functions::_DB_FindXAssetHeader));
	MH_EnableHook(reinterpret_cast<void*>(0xFAB20_b));

	LuiLoader::init();
	FontLoader::init();
	ScriptLoader::init();
}
