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

void* hook_Image_Setup(GfxImage* image, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipCount, uint32_t imageFlags, DXGI_FORMAT imageFormat, const char* name, const void* initData) {
    //Console::printf("Image_Setup: %s, width: %d, height: %d, depth: %d, mipCount: %d, imageFlags: %d", name, width, height, depth, mipCount, imageFlags);
    return _Image_Setup(image, width, height, depth, mipCount, imageFlags, imageFormat, name, initData);
 
}

void imageThing() {
    MH_CreateHook(reinterpret_cast<void*>(0x90B9D0_b), &hook_Image_Setup, reinterpret_cast<void**>(&_Image_Setup));
    MH_EnableHook(reinterpret_cast<void*>(0x90B9D0_b));
}

void DevPatches::imageTestPt2() {

}

void DevPatches::init()  {
    DEV_INIT_PRINT();
    imageThing();
}