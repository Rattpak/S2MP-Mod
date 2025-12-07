#pragma once
#include "Console.hpp"
#include "structs.h"
#include "GameUtil.hpp"
#include "MinHook.h"

class Loaders {
public:
	static void initAssetLoaders();
};

class LuiLoader {
public:
	static void init();
};

class ScriptLoader {
public:
	static void init();
};

class FontLoader {
public:
	static void init();
};

class StringTableLoader {
public:
	static void dump(StringTable* table);
	static void loadCustom(StringTable* table);
};

class RawFileLoader {
public:
	static void dump(RawFile* rawfile);
	static void loadCustom(RawFile* rawfile);
};

class MapEntLoader {
public:
	static void dump(RawFile* rawfile);
};