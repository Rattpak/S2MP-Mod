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

//TODO: run script errors thru the same error map
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
    {840, "LUI: Out of memory"},
    {2997, "StopDynamicAmbience needs 1 parameter - you had %i parameters"},
    {4516, "not an entity"},
    {4515, "entity %u is not a player"},
    {3976, "Could not find menu ID for '%s'"},
    {3978, "Could not find menu ID for '%s'"},
    {3979, "Could not find menu ID for '%s'"},
    {1624, "USAGE: <player> Scr_VisionSetStage( <stage>, <duration> )"},
    {3288, "Player provided is not a client or an agent."},
    {3290, "Invalid slot number %i, must be between 0 and %i."},
    {4517, "entity %u is not a player or agent"},
    {1599, "%s is an invalid dvar name"},
    {1592, "SetDvar: unset dvar '%s' cannot be set as a client dvar. need to be added to the config file. This won't work in a future build"},
    {1593, "server dvar '%s' cannot be set as a client dvar"},
    {1594, "non-writable dvar '%s' cannot be set as a client dvar"},
    {1594, "unknown network id for dvar '%s'"},
    {3984, "Duration must be positive"},
    {3987, "Duration must be positive"},
    {1632, "USAGE: <player> SetClutForPlayer( <clut name>, <duration> )"},
    {1633, "USAGE: <player> SetClutOverrideEnableForPlayer( <clut name>, <duration> )"},
    {4081, "LUIOpenMenu: Must be called on a player entity."},
    {2484, "LUIOpenMenu: Need menu name."},
    {2485, "LUIOpenMenu: Menu name (string) is not precached."},
    {599, "radius not specified for trigger_radius at (%g %g %g)"},
    {1636, "SetBlurForPlayer - Omnvars not found - blur_target or blur_duration_ms"},
    {3990, "Blur value must be greater than 0"},
    {3989, "Time must be positive"},
    {4519, "entity %i is not a player or agent"},
    {1289, "SAVE_STRING_MAX_SIZE exceeded in save game"},
    {1290, "SAVE_STRING_MAX_SIZE exceeded in save game"},
    {224, "Unable to find the lookup table 'mp/attachmenttable.csv' in the fastfile"},
    {659, "SV_LoadHistoryState: objectivenum out of range (%i, MAX = %i)"},
    {660, "SV_LoadHistoryState: entitynum out of range (%i, MAX = %i)"},
    {661, "SV_LoadHistoryState: clientnum out of range"},
    {662, "SV_LoadHistoryState: agentnum out of range"},
    {663, "SV_LoadHistoryState: entnum out of range: %i"},
    {664, "SV_LoadHistoryState: DObj expected for entnum %i"},
    {2225, "Source Damage vector is invalid : %d %d %d"},
    {4081, "Usage: doDamage( <health>, <source position>, <attacker>, <inflictor> )"},
    {3585, "SetGenericField is attempting to truncate a negative number into an F_USHORT (which is unsigned)"},
    {3586, "SetGenericField is attempting to truncate a negative number into an F_BYTE (which is unsigned)"},
    {1727, "Usage: waypoint SetHideTrigger( trigger_ent )"},
    {4005, "not a hud element"},
    {3270, "invalid SetSpawnWeapon: %s"},
    {375, "Weapon index mismatch for '%s'"},
    {3403, "Not enough parameters for Kick()"},
    {4564, "Invalid client specified (%i) out of %i clients"},
    {4563, "Localized string should start with %s"},
    {3390, "exitlevel already called"},
    {3388, "map_restart already called"},
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