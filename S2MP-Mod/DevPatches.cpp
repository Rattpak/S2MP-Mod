/////////////////////////////////////////////////
//         Development Patches
// Temporary patches for testing stuff
/////////////////////////////////////////////////

#include "pch.h"
#include "DevPatches.hpp"
#include "Console.hpp"
#include <MinHook.h>
#include "FuncPointers.h"
#include <winternl.h>
#include <psapi.h>
#include <algorithm>
#include "GameUtil.hpp"
uintptr_t DevPatches::base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;

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

                if (CreateProcessW(nullptr, commandLine, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
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


typedef void(__fastcall* LUI_OpenMenu_t)(int client, const char* menu, int a, uint32_t is_exclusive);
static LUI_OpenMenu_t _LUI_OpenMenu = nullptr;

void __fastcall hk_LUI_OpenMenu(int client, const char* menu, int a, uint32_t is_exclusive) {
    if (menu) {
        Console::devPrint("LUI_OpenMenu: " + std::string(menu));
    }
        
    _LUI_OpenMenu(client, menu, a, is_exclusive);
}

void Hook_LUI_OpenMenu() {
    void* target = (void*)(GameUtil::base + 0x740A30);

    if (MH_Initialize() != MH_OK)
        return;

    if (MH_CreateHook(target, &hk_LUI_OpenMenu, reinterpret_cast<void**>(&_LUI_OpenMenu)) != MH_OK)
        return;

    if (MH_EnableHook(target) != MH_OK)
        return;
}

void DevPatches::init() {
    Console::initPrint("DevPatches::init()");
    HookNtCreateUserProcess();
    Hook_LUI_OpenMenu();
}