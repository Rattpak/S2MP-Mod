//////////////////////////////////////
//            Errors
//	Mapping out the stripped format 
// strings to the actual error formats
// 
// Very useful for zombies community
//////////////////////////////////////
#include "pch.h"
#include "Errors.hpp"
#include "Console.hpp"

static std::unordered_map<int, std::string> errorMap = {
    {999, "Trying to set persistent player data while connected to a server (%s, clcState = %i)!"},
    {719, "R_RegisterFont: Too many TTF fonts registered (%d)."},
    {1278, "Unable to compare against column number %i - there are only %i columns"},
    {845, "LUI ERROR: Failed to allocate from LUITween pool. Restarting the Lua VM"},
    {674, "G_Spawn: no free entities"},
    {560, "weapDef: No world model loaded for entity %i with model %s"},
    {879, "CM_LoadMap: NULL name"},
    {1274, "ERROR: Invalid server value '%s' for '%s'"},
    {1260, "FS_BuildOSPath: os path length exceeded"},
    {447, "Too many active clients.  You may be missing a \"nosplitscreen\" in a menu."},
    //if anyone finds more just add them here
};

typedef void(*Com_Error)(errorParm_t code, const char* fmt, ...);
Com_Error _Com_Error = nullptr;


void hook_Com_Error(errorParm_t code, const char* fmt, ...) {
    std::string fmtStr(fmt);
    std::istringstream iss(fmtStr);
    std::string firstToken;
    iss >> firstToken;

    int errorId = -1;
    bool isNumber = !firstToken.empty() && std::all_of(firstToken.begin(), firstToken.end(), ::isdigit);

    //this is the main error string replacement thingy
    if (isNumber) {
        errorId = std::stoi(firstToken);
        auto it = errorMap.find(errorId);
        if (it != errorMap.end()) {
            fmtStr = it->second;
        }
    }

    va_list args;
    va_start(args, fmt);

    //needed copy cuz original was getting cleared
    va_list argsCopy;
    va_copy(argsCopy, args);

    int needed = vsnprintf(nullptr, 0, fmtStr.c_str(), args);
    va_end(args);

    if (needed < 0) {
        _Com_Error(code, fmtStr.c_str());
        va_end(argsCopy);
        return;
    }

    std::vector<char> buf(needed + 1);
    vsnprintf(buf.data(), buf.size(), fmtStr.c_str(), argsCopy);
    va_end(argsCopy);

    //taken from t6 so idk if this is SHG accurate
    Console::print("********************");
    Console::printf("ERROR: %s", buf.data());
    Console::print("********************");


    _Com_Error(code, "%s", buf.data());
}


void Errors::init() {
	Console::initPrint(__FUNCTION__);
    MH_CreateHook(reinterpret_cast<void*>(0xAA1E0_b), &hook_Com_Error, reinterpret_cast<void**>(&_Com_Error));
    MH_EnableHook(reinterpret_cast<void*>(0xAA1E0_b));
}