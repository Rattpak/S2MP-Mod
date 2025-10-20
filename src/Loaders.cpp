#include "pch.h"
#include "Loaders.hpp"
#include "FuncPointers.h"

typedef XAssetHeader(*DB_FindXAssetHeader)(XAssetType type, const char* name, int allow_create_default);
DB_FindXAssetHeader _DB_FindXAssetHeader = nullptr;
XAssetHeader hook_DB_FindXAssetHeader(XAssetType type, const char* name, int allow_create_default) {
    auto start = std::chrono::high_resolution_clock::now();
    const char* safeName = (name && name[0]) ? name : "<null>";
    XAssetHeader header = _DB_FindXAssetHeader(type, name, allow_create_default);
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (dur > 100) {
        Console::printf("Waited %i msec for asset '%s'", dur, safeName);
    }

    switch (type) {
    case ASSET_TYPE_STRINGTABLE: {
        // always dump before load
        if (Functions::_Dvar_FindVar("g_dumpStringTables")->current.enabled) {
            StringTableLoader::dump(header.table);
        }
        StringTableLoader::loadCustom(header.table);
        break;
    }
    case ASSET_TYPE_RAWFILE: {
        if (Functions::_Dvar_FindVar("g_dumpRawfiles")->current.enabled) {
            RawFileLoader::dump(header.rawfile);
        }
        RawFileLoader::loadCustom(header.rawfile);
        break;
    }
    default: { 
        break;
    }
    }

    return header;
}


void Loaders::initAssetLoaders() {
	//DB_FindXAssetHeader hook
	MH_CreateHook(reinterpret_cast<void*>(0xFAB20_b), &hook_DB_FindXAssetHeader, reinterpret_cast<void**>(&_DB_FindXAssetHeader));
	MH_EnableHook(reinterpret_cast<void*>(0xFAB20_b));

	LuiLoader::init();
	FontLoader::init();
	ScriptLoader::init();
}
