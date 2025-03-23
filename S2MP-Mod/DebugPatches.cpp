///////////////////////////////////////////
//         Debug Patches
//	Patching some anti-tamper stuff
////////////////////////////////////////////
#include "pch.h"
#include "DebugPatches.hpp"
#include "Console.hpp"
#include "GameUtil.hpp"
#include <MinHook.h>
#include <Windows.h>
#include <array>
#include <mutex>
#include <winternl.h>
#include <TlHelp32.h>
#include <string>

std::string DebugPatches::conLabel = "DP";
uintptr_t DebugPatches::base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef NTSTATUS(WINAPI* NtUserBuildHwndList_t)(HDESK hdesk, HWND hwndParent, BOOL fChildren, BOOL fOwner, DWORD dwThreadId, UINT cHwndMax, HWND* phwnd, PUINT pcHwndNeeded);
NtUserBuildHwndList_t fpNtUserBuildHwndList = nullptr;

std::vector<std::string> evilStrings = {
    "cheatengine", "x64dbg", "x32dbg", "cheat", "debug", "ida"
};

//check for evil strings
bool shouldFilterEvilStrings(HWND hwnd) {
    char windowTitle[256] = { 0 };
    if (GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle)) > 0) {
        std::string title(windowTitle);
        for (const auto& bad : evilStrings) {
            if (title.find(bad) != std::string::npos) {
                return true;
            }
        }
    }
    return false;
}

NTSTATUS WINAPI HookedNtUserBuildHwndList(HDESK hdesk, HWND hwndParent, BOOL fChildren, BOOL fOwner, DWORD dwThreadId,UINT cHwndMax, HWND* phwnd, PUINT pcHwndNeeded) {
    NTSTATUS result = fpNtUserBuildHwndList(hdesk, hwndParent, fChildren, fOwner, dwThreadId, cHwndMax, phwnd, pcHwndNeeded);

    if (NT_SUCCESS(result) && phwnd != nullptr && pcHwndNeeded != nullptr) {
        UINT count = *pcHwndNeeded;
        std::vector<HWND> filteredHwnds;

        for (UINT i = 0; i < count; i++) {
            if (!shouldFilterEvilStrings(phwnd[i])) {
                filteredHwnds.push_back(phwnd[i]);
            }
        }

        //copy filtered hwnd list
        UINT newSize = static_cast<UINT>(filteredHwnds.size());
        std::memcpy(phwnd, filteredHwnds.data(), newSize * sizeof(HWND));
        *pcHwndNeeded = newSize;
    }

    return result;
}

void bypassHwndChecks() {
    Console::labelPrint(DebugPatches::conLabel, "Patching HWND Checks");
    HMODULE hUser32 = LoadLibraryA("win32u.dll");
    if (!hUser32) {
        Console::print("Failed to load win32u.dll");
        return;
    }

    FARPROC pFunc = GetProcAddress(hUser32, "NtUserBuildHwndList");
    if (!pFunc) {
        Console::print("Failed to get address of NtUserBuildHwndList");
        return;
    }

    if (MH_CreateHook(pFunc, &HookedNtUserBuildHwndList, reinterpret_cast<LPVOID*>(&fpNtUserBuildHwndList)) != MH_OK) {
        Console::print("Failed to create hook");
        return;
    }

    if (MH_EnableHook(pFunc) != MH_OK) {
        Console::print("Failed to enable hook");
        return;
    }
}

void DebugPatches::init() {
    Console::initPrint("DebugPatches::init()");
    bypassHwndChecks();
}
