#include "pch.h"
#include "Loaders.hpp"

typedef void(*Load_ScriptFileAsset)(ScriptFile** scriptFile);
Load_ScriptFileAsset _Load_ScriptFileAsset = nullptr;

void dumpScript(ScriptFile* script) {
	std::string filePath = "S2MP-Mod/dump/scripts/" + std::string(script->name) + ".gscbin";
	//get directory path from filePath
	std::filesystem::path dirPath = std::filesystem::path(filePath).parent_path();
	if (!std::filesystem::exists(dirPath)) {
		std::filesystem::create_directories(dirPath);
	}

	std::ofstream outFile(filePath, std::ios::out | std::ios::binary);
	if (!outFile) {
		Console::labelPrint("File path error for: %s", filePath.c_str());
		return;
	}
	outFile.write("GSC\0", 4); //magic
	outFile.write(reinterpret_cast<const char*>(&script->compressedLen), sizeof(int));
	outFile.write(reinterpret_cast<const char*>(&script->len), sizeof(int));
	outFile.write(reinterpret_cast<const char*>(&script->bytecodeLen), sizeof(int));
	outFile.write(script->buffer, script->compressedLen);
	outFile.write(script->bytecode, script->bytecodeLen);
	outFile.close();
	Console::printf("Script Dumped to: %s", filePath.c_str());
}

void hook_Load_ScriptFileAsset(ScriptFile** scriptFile) {
	if (scriptFile && *scriptFile) {
		ScriptFile* varScriptFile = *scriptFile;
		if (varScriptFile->buffer) {
			//Console::printf("Loading ScriptFile '%s' with compressedLen of: %d", varScriptFile->name, varScriptFile->compressedLen);
			//dumpScript(varScriptFile);

		}
	}
	else {
		Console::printf("Null ScriptFile passed to %s", __FUNCTION__);
	}
	
    _Load_ScriptFileAsset(scriptFile);
}

void ScriptLoader::init() {
	MH_CreateHook(reinterpret_cast<void*>(0x13CC70_b), &hook_Load_ScriptFileAsset, reinterpret_cast<void**>(&_Load_ScriptFileAsset));
	MH_EnableHook(reinterpret_cast<void*>(0x13CC70_b));
}