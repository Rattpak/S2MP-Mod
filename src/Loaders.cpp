#include "pch.h"
#include "Loaders.hpp"

void Loaders::initAssetLoaders() {
	LuiLoader::init();
	FontLoader::init();
	ScriptLoader::init();
}
