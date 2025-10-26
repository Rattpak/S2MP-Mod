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



typedef NTSTATUS(WINAPI* NtCreateUserProcess_t)(PHANDLE ProcessHandle, PHANDLE ThreadHandle, ACCESS_MASK ProcessDesiredAccess, ACCESS_MASK ThreadDesiredAccess, POBJECT_ATTRIBUTES ProcessObjectAttributes,
    POBJECT_ATTRIBUTES ThreadObjectAttributes, ULONG ProcessFlags, ULONG ThreadFlags, PVOID ProcessParameters, PVOID CreateInfo, PVOID AttributeList);

NtCreateUserProcess_t fpNtCreateUserProcess = nullptr;

NTSTATUS WINAPI HookedNtCreateUserProcess(PHANDLE ProcessHandle, PHANDLE ThreadHandle, ACCESS_MASK ProcessDesiredAccess, ACCESS_MASK ThreadDesiredAccess, POBJECT_ATTRIBUTES ProcessObjectAttributes,
    POBJECT_ATTRIBUTES ThreadObjectAttributes, ULONG ProcessFlags, ULONG ThreadFlags, PVOID ProcessParameters, PVOID CreateInfo, PVOID AttributeList) {

    ACCESS_MASK newProcessAccess = ProcessDesiredAccess | PROCESS_QUERY_LIMITED_INFORMATION;
    NTSTATUS status = fpNtCreateUserProcess(ProcessHandle, ThreadHandle, newProcessAccess, ThreadDesiredAccess, ProcessObjectAttributes, ThreadObjectAttributes, ProcessFlags, ThreadFlags, ProcessParameters, CreateInfo, AttributeList);

    if (NT_SUCCESS(status)) {
        HANDLE hProcess = *ProcessHandle;
        WCHAR imagePath[MAX_PATH];
        DWORD result = GetProcessImageFileNameW(hProcess, imagePath, MAX_PATH);

        if (result > 0) {
            std::wstring path(imagePath, result);
            size_t lastBackslash = path.find_last_of(L'\\');
            std::wstring fileName = (lastBackslash != std::wstring::npos) ? path.substr(lastBackslash + 1) : path;

            //TODO: use gameutil one maybe
            std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::towlower);

            if (fileName == L"s2_mp64_ship.exe") {
                WCHAR commandLine[] = L"\"S2MP-Mod-Launcher.exe\" -noexec"; //noexec is only for this exact purpose

                STARTUPINFOW si = { sizeof(si) };
                PROCESS_INFORMATION pi;

                if (CreateProcessW(nullptr, commandLine, nullptr, nullptr, 0, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                }
            }
        }
    }
    return status;
}

void HookNtCreateUserProcess() {
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        Console::print("Failed to load ntdll.dll");
        return;
    }

    FARPROC pFunc = GetProcAddress(hNtdll, "NtCreateUserProcess");
    if (!pFunc) {
        Console::print("Failed to get address of NtCreateUserProcess");
        return;
    }

    MH_CreateHook(pFunc, &HookedNtCreateUserProcess, reinterpret_cast<LPVOID*>(&fpNtCreateUserProcess));
    MH_EnableHook(pFunc);
}

typedef void (*Cmd_AddCommandInternal_t)(const char* name, void* func, cmd_function_t* cmd);
static Cmd_AddCommandInternal_t oCmd_AddCommandInternal = nullptr;

void Cmd_AddCommandInternal_hookfunc(const char* name, void* func, cmd_function_t* cmd) {
    if (name) {
        Console::devPrint("Adding Cmd: " + std::string(name));
    }

    oCmd_AddCommandInternal(name, func, cmd);
}

void Hook_Cmd_AddCommandInternal() {
    void* target = (void*)(0x6AE0E0_b);

    if (MH_CreateHook(target, &Cmd_AddCommandInternal_hookfunc, reinterpret_cast<void**>(&oCmd_AddCommandInternal)) != MH_OK) {
        Console::devPrint("ERROR: MH_CreateHook failure in function " + std::string(__FUNCTION__));
        return;
    }

    if (MH_EnableHook(target) != MH_OK) {
        Console::devPrint("ERROR: MH_EnableHook failure in function " + std::string(__FUNCTION__));
        return;
    }

}



typedef void*(*Image_Setup)(GfxImage* image, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipCount, uint32_t imageFlags, DXGI_FORMAT imageFormat, const char* name, const void* initData);
Image_Setup _Image_Setup = nullptr;

void* hook_Image_Setup(GfxImage* image, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipCount, uint32_t imageFlags, DXGI_FORMAT imageFormat, const char* name, const void* initData) {
    Console::printf("Image_Setup: %s, width: %d, height: %d, depth: %d, mipCount: %d, imageFlags: %d", name, width, height, depth, mipCount, imageFlags);
    return _Image_Setup(image, width, height, depth, mipCount, imageFlags, imageFormat, name, initData);
 
}

void imageThing() {
    MH_CreateHook(reinterpret_cast<void*>(0x90B9D0_b), &hook_Image_Setup, reinterpret_cast<void**>(&_Image_Setup));
    MH_EnableHook(reinterpret_cast<void*>(0x90B9D0_b));
}

void DevPatches::imageTestPt2() {

}

void DevPatches::init()  {
#ifdef DEVELOPMENT_BUILD
    Console::initPrint(__FUNCTION__);
#endif
    imageThing();
    //HookNtCreateUserProcess();
}