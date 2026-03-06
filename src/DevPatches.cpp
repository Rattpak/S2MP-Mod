/////////////////////////////////////////////////
//         Development Patches
// Temporary patches for testing stuff
/////////////////////////////////////////////////

#include "pch.h"
#include "DevPatches.hpp"
#include "Console.hpp"
#include "FuncPointers.h"
#include "GameUtil.hpp"
#include "memory.h"
#include <DevDef.h>
#include <xsk/gsc/engine/s2.hpp>
#include <Hook.hpp>
#include <string.h>

typedef void*(*Image_Setup)(GfxImage* image, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipCount, uint32_t imageFlags, DXGI_FORMAT imageFormat, const char* name, const void* initData);
Image_Setup _Image_Setup = nullptr;

typedef int (*LiveStorage_GetItemLockStatus)(int, unsigned int);
LiveStorage_GetItemLockStatus _LiveStorage_GetItemLockStatus = nullptr;

void* hook_Image_Setup(GfxImage* image, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipCount, uint32_t imageFlags, DXGI_FORMAT imageFormat, const char* name, const void* initData) {
    //Console::printf("Image_Setup: %s, width: %d, height: %d, depth: %d, mipCount: %d, imageFlags: %d", image->name, width, height, depth, mipCount, imageFlags);
    return _Image_Setup(image, width, height, depth, mipCount, imageFlags, imageFormat, name, initData);
 
}

void imageThing() {
    MH_CreateHook(reinterpret_cast<void*>(0x90B9D0_b), &hook_Image_Setup, reinterpret_cast<void**>(&_Image_Setup));
    MH_EnableHook(reinterpret_cast<void*>(0x90B9D0_b));
}

void otherUnlockAllSetup() {
    DEV_ONLY_FUNCTION();
    constexpr std::array<unsigned char, 3> PATCH_BYTES = { 0x31, 0xC0, 0xC3 }; //da patch
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)(0x1334A0_b), PATCH_BYTES.data(), PATCH_BYTES.size(), nullptr); //LiveStorage_IsItemUnlockedFromTable
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)(0x7A68B0_b), PATCH_BYTES.data(), PATCH_BYTES.size(), nullptr); //GetIsItemUnlockedStr

}

bool wasUnlockAllOn = false;
int hook_LiveStorage_GetItemLockStatus(int f, unsigned int g) {
    DEV_ONLY_FUNCTION();
    if (Functions::_Dvar_FindVar("unlockAllItems")->current.enabled) {
        if (!wasUnlockAllOn) {
            otherUnlockAllSetup();
        }
        wasUnlockAllOn = true;
        return 0;
    }
    else {
        if (wasUnlockAllOn) {
            Functions::_Dvar_FindVar("unlockAllItems")->current.enabled = true;
        }
        return _LiveStorage_GetItemLockStatus(f, g);
    }
}


void unlockallSetup() {
    MH_CreateHook(reinterpret_cast<void*>(0x132510_b), &hook_LiveStorage_GetItemLockStatus, reinterpret_cast<void**>(&_LiveStorage_GetItemLockStatus));
    MH_EnableHook(reinterpret_cast<void*>(0x132510_b));

}

#ifdef DEVELOPMENT_BUILD

bool isEulaName(const char* name) {
    const char* prefix = "EULA_";
    size_t len = strlen(prefix);

    if (strncmp(name, prefix, len) != 0) {
        return false;
    }

    if (!isdigit(name[len])) {
        return false;
    }

    for (const char* p = name + len; *p; ++p) {
        if (!isdigit(*p))
            return false;
    }

    return true;
}

#ifdef DEVELOPMENT_BUILD
void DevPatches::doEulaTest() {
    DEV_ONLY_FUNCTION();

    int localclientprofile = 0;
    uint8_t* base = reinterpret_cast<uint8_t*>(0x8DD0C90_b);
    uint8_t* profile = base + (6920 * localclientprofile);
    profile[118] = 0;//HasAccepted_EULA
}
#endif // DEVELOPMENT_BUILD


const char* eula_header = "S2MP-Mod Development Build Notes";
const char* eula_subheader = "";
std::string eula_subheader_str;
const char* eula_exit = "You must accept the S2MP-Mod Development Build notes to proceed.";
const char* eula_page1 = "Thank you for testing this S2MP-Mod Development (Alpha) Build. Both Zombies and Multiplayer are playable with this build.\n\n"
                         "Whats new in this build:\n\n"
                         " - Aim Assist prototype\n"
                         "      + Auto Aim (aim snapping) is forced on in this build. Will be changed later.\n"
                         "      + Aim Assist Lockon and Aim Assist Slowdown can be enabled using aim_lockon_enabled dvar\n"
                         "      + Aim Assist will probably be very strong right now\n\n"
                         " - Mapped some sound Dvars\n\n"
                         " - Mapped some safe area Dvars\n\n"
                         " - More error strings. Removed overlapping error strings.\n\n"
                         " - Unlock All is still W.I.P\n"
                         "      + It is FORCED on in this build until crashing issue is solved\n\n"
                         "\n\n\nWarning: Development builds may not be stable! Expect crashes! Please report any bugs in the discord server and send any crash reports to Rattpak";
bool DevPatches::customLocStr(const char* name, XAssetHeader& header) {
    if (eula_subheader[0] == '\0') { //one time
        eula_subheader_str = DevDraw::getDevBuildDate();
        eula_subheader = eula_subheader_str.c_str();
    }
   

    if (!strcmp(name, "EULA_HEADING")) {
        LocalizeEntry* entry = new LocalizeEntry{};
        entry->value = eula_header;
        entry->name = "EULA_HEADING";
        header.localize = entry;
        return true;
    }

    if (!strcmp(name, "EULA_SUBHEADING")) {
        LocalizeEntry* entry = new LocalizeEntry{};
        entry->value = eula_subheader;
        entry->name = "EULA_SUBHEADING";
        header.localize = entry;
        return true;
    }

    if (isEulaName(name)) {
        LocalizeEntry* entry = new LocalizeEntry{};
        entry->value = eula_page1; //always the same value
        entry->name = name; //keep original name
        header.localize = entry;
        return true;
    }

    if (!strcmp(name, "PLATFORM_EULA_PROMPT")) {
        LocalizeEntry* entry = new LocalizeEntry{};
        entry->value = eula_exit;
        entry->name = "PLATFORM_EULA_PROMPT";
        header.localize = entry;
        return true;
    }

    return false;
}
#endif // DEVELOPMENT_BUILD


void enableMpAimAssist() {
    DEV_ONLY_FUNCTION();
    constexpr std::array<unsigned char, 2> NOP_BYTES = { 0x90, 0x90 }; //patch
    
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)(0x3BFE9_b), NOP_BYTES.data(), NOP_BYTES.size(), nullptr); 

}

void DevPatches::imgtest() {
    CmdArgs* cmdArgs = GameUtil::getCmdArgs();
    if (!cmdArgs) {
        return;
    }

    int nest = cmdArgs->nesting;
    int count = cmdArgs->argc[nest];
    if (count != 2) {
        Console::printf("Usage: imgtest <image>");
        return;
    }

    const char** args = cmdArgs->argv[nest];
    std::string imgname = std::string(args[1]);

    GfxImage* image = Functions::_DB_FindXAssetHeader(ASSET_TYPE_IMAGE, args[1], 0).image;
    if (!image) {
        Console::printf("Image does not exist");
        return;
    }
    Console::printf("image->name: %s", image->name);
    Console::printf("image->width: %hu", image->width);
    Console::printf("image->height: %hu", image->height);
    std::ofstream out("S2MP-Mod-DEV/" + imgname + "_dump.bin", std::ios::binary);
    out.write(reinterpret_cast<const char*>(image), 0x60);
    out.close();
}

void DevPatches::init()  {
    DEV_INIT_PRINT();
    imageThing();
    unlockallSetup();
    enableMpAimAssist();
    GameUtil::Cbuf_AddText(LOCAL_CLIENT_0, "telemetry_active 0");
}