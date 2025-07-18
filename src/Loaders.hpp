#pragma once
#include "Console.hpp"
#include "client/game/structs.h"
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

class FontLoader {
public:
	static void init();
};