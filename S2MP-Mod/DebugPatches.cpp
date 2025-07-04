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

#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004
#define ThreadHideFromDebugger 0x11

std::string DebugPatches::conLabel = "DP";
uintptr_t DebugPatches::base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef NTSTATUS(WINAPI* NtUserBuildHwndList_t)(HDESK hdesk, HWND hwndParent, BOOL fChildren, BOOL fOwner, DWORD dwThreadId, UINT cHwndMax, HWND* phwnd, PUINT pcHwndNeeded);
NtUserBuildHwndList_t fpNtUserBuildHwndList = nullptr;

typedef BOOL(WINAPI* CheckRemoteDebuggerPresent_t)(HANDLE hProcess, PBOOL pbDebuggerPresent);
CheckRemoteDebuggerPresent_t fpCheckRemoteDebuggerPresent = nullptr;

typedef NTSTATUS(WINAPI* NtCreateThreadEx_t)(OUT PHANDLE ThreadHandle, IN ACCESS_MASK DesiredAccess, IN PVOID ObjectAttributes, IN HANDLE ProcessHandle, IN PVOID StartRoutine, IN PVOID Argument, 
    IN ULONG CreateFlags, IN SIZE_T ZeroBits, IN SIZE_T StackSize, IN SIZE_T MaximumStackSize, IN PVOID AttributeList);
NtCreateThreadEx_t fpNtCreateThreadEx = nullptr;

typedef NTSTATUS(WINAPI* NtSetInformationThread_t)(HANDLE ThreadHandle, THREAD_INFORMATION_CLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength);
NtSetInformationThread_t fpNtSetInformationThread = nullptr;

typedef NTSTATUS(WINAPI* NtQueryInformationThread_t)(HANDLE ThreadHandle, THREAD_INFORMATION_CLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength);
NtQueryInformationThread_t fpNtQueryInformationThread = nullptr;

//these are the functions Arxan uses to break debugging
const char* kPatchedFuncs[] = {
    "DbgBreakPoint", "DbgUserBreakPoint", "DbgUiConnectToDbg", "DbgUiContinue",
    "DbgUiConvertStateChangeStructure", "DbgUiDebugActiveProcess", "DbgUiGetThreadDebugObject",
    "DbgUiIssueRemoteBreakin", "DbgUiRemoteBreakin", "DbgUiSetThreadDebugObject",
    "DbgUiStopDebugging", "DbgUiWaitStateChange", "DbgPrintReturnControlC", "DbgPrompt"
};

std::vector<std::string> evilStrings = {
    "cheatengine", "x64dbg", "x32dbg", "cheat", "debug", "ida"
};

//helper to restore the first N bytes of a function
bool restoreFunctionPrologue(const char* moduleName, const char* funcName, size_t numBytes = 14) {
    HMODULE localModule = GetModuleHandleA(moduleName);
    if (!localModule) {
        Console::print(std::string("Failed to get local module: ") + moduleName);
        return false;
    }

    //get the clean function address from this process
    FARPROC cleanFunc = GetProcAddress(localModule, funcName);
    if (!cleanFunc) {
        Console::print(std::string("Failed to get clean func: ") + funcName);
        return false;
    }

    //copy the first N bytes into buffer
    BYTE originalBytes[32] = { 0 };
    memcpy(originalBytes, cleanFunc, numBytes);

    //unprotect memory
    DWORD oldProtect;
    if (!VirtualProtect((LPVOID)cleanFunc, numBytes, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        Console::labelPrint(DebugPatches::conLabel, std::string("Failed to unprotect memory: ") + funcName);
        return false;
    }

    //restore the bytes
    memcpy((void*)cleanFunc, originalBytes, numBytes);
    VirtualProtect((LPVOID)cleanFunc, numBytes, oldProtect, &oldProtect);
    //Console::labelPrint(DebugPatches::conLabel, std::string("Restored: ") + funcName);
    return true;
}

void restoreAllPatchedFunctions() {
    Console::labelPrint(DebugPatches::conLabel, "Restoring debugger-blocked functions...");
    for (const auto& funcName : kPatchedFuncs) {
        restoreFunctionPrologue("ntdll.dll", funcName);
    }

    //CopyFileExW
    restoreFunctionPrologue("kernel32.dll", "CopyFileExW");
}

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

NTSTATUS WINAPI HookedNtQueryInformationThread(
    HANDLE ThreadHandle,
    THREAD_INFORMATION_CLASS ThreadInformationClass,
    PVOID ThreadInformation,
    ULONG ThreadInformationLength,
    PULONG ReturnLength
) {
    if (ThreadInformationClass == (THREAD_INFORMATION_CLASS)ThreadHideFromDebugger) {
        Console::print("Blocked NtQueryInformationThread(ThreadHideFromDebugger)");
        return 0xC00000BB; //STATUS_NOT_SUPPORTED
    }

    return fpNtQueryInformationThread(
        ThreadHandle,
        ThreadInformationClass,
        ThreadInformation,
        ThreadInformationLength,
        ReturnLength
    );
}


BOOL WINAPI HookedCheckRemoteDebuggerPresent(HANDLE hProcess, PBOOL pbDebuggerPresent) {
    BOOL result = fpCheckRemoteDebuggerPresent(hProcess, pbDebuggerPresent);
    if (pbDebuggerPresent != nullptr) {
        if (pbDebuggerPresent) {
            Console::labelPrint(DebugPatches::conLabel, "CheckRemoteDebuggerPresent called");
        }
        *pbDebuggerPresent = FALSE;
    }

    return result;
}

NTSTATUS WINAPI HookedNtCreateThreadEx(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, PVOID ObjectAttributes, HANDLE ProcessHandle, PVOID StartRoutine, PVOID Argument, ULONG CreateFlags,
SIZE_T ZeroBits, SIZE_T StackSize, SIZE_T MaximumStackSize, PVOID AttributeList) {
    //remove HIDE_FROM_DEBUGGER flag
    if (CreateFlags & THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER) {
        CreateFlags &= ~THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER;
        Console::labelPrint(DebugPatches::conLabel, "Stripped THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER from thread creation");
    }

    return fpNtCreateThreadEx(ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle, StartRoutine, Argument, CreateFlags,
        ZeroBits, StackSize, MaximumStackSize, AttributeList);
}

void bypassHiddenThreadCreation() {
    Console::labelPrint(DebugPatches::conLabel, "Patching NtCreateThreadEx");

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        Console::print("Failed to get handle for ntdll.dll");
        return;
    }

    FARPROC pFunc = GetProcAddress(hNtdll, "NtCreateThreadEx");
    if (!pFunc) {
        Console::print("Failed to get address of NtCreateThreadEx");
        return;
    }

    if (MH_CreateHook(pFunc, &HookedNtCreateThreadEx, reinterpret_cast<LPVOID*>(&fpNtCreateThreadEx)) != MH_OK) {
        Console::print("Failed to create hook for NtCreateThreadEx");
        return;
    }

    if (MH_EnableHook(pFunc) != MH_OK) {
        Console::print("Failed to enable hook for NtCreateThreadEx");
        return;
    }
}
NTSTATUS WINAPI HookedNtSetInformationThread(HANDLE ThreadHandle, THREAD_INFORMATION_CLASS ThreadInformationClass, PVOID ThreadInformation,ULONG ThreadInformationLength) {
    //block attempts to hide thread from debooger
                                      //ThreadHideFromDebugger v
    if (ThreadInformationClass == (THREAD_INFORMATION_CLASS)ThreadHideFromDebugger) {
        Console::labelPrint(DebugPatches::conLabel, "Blocked ThreadHideFromDebugger via NtSetInformationThread");
        return 0; //success
    }

    return fpNtSetInformationThread(ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength);
}

void antiDebuggerSyscalls() {
    Console::labelPrint(DebugPatches::conLabel, "Patching CheckRemoteDebuggerPresent");

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32) {
        Console::print("Failed to get handle for kernel32.dll");
        return;
    }

    FARPROC pFunc = GetProcAddress(hKernel32, "CheckRemoteDebuggerPresent");
    if (!pFunc) {
        Console::print("Failed to get address of CheckRemoteDebuggerPresent");
        return;
    }

    if (MH_CreateHook(pFunc, &HookedCheckRemoteDebuggerPresent, reinterpret_cast<LPVOID*>(&fpCheckRemoteDebuggerPresent)) != MH_OK) {
        Console::print("Failed to create hook for CheckRemoteDebuggerPresent");
        return;
    }

    if (MH_EnableHook(pFunc) != MH_OK) {
        Console::print("Failed to enable hook for CheckRemoteDebuggerPresent");
        return;
    }
}

void bypassThreadHideFromDebugger() {
    Console::labelPrint(DebugPatches::conLabel, "Patching ThreadHideFromDebugger");

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        Console::print("Failed to get handle for ntdll.dll");
        return;
    }

    FARPROC pFunc = GetProcAddress(hNtdll, "NtSetInformationThread");
    if (!pFunc) {
        Console::print("Failed to get address of NtSetInformationThread");
        return;
    }

    if (MH_CreateHook(pFunc, &HookedNtSetInformationThread, reinterpret_cast<LPVOID*>(&fpNtSetInformationThread)) != MH_OK) {
        Console::print("Failed to create hook for NtSetInformationThread");
        return;
    }

    if (MH_EnableHook(pFunc) != MH_OK) {
        Console::print("Failed to enable hook for NtSetInformationThread");
        return;
    }
}
void bypassThreadQueryHideFromDebugger() {
    Console::labelPrint(DebugPatches::conLabel, "Patching NtQueryInformationThread");

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        Console::print("Failed to get handle for ntdll.dll");
        return;
    }

    FARPROC pFunc = GetProcAddress(hNtdll, "NtQueryInformationThread");
    if (!pFunc) {
        Console::print("Failed to get address of NtQueryInformationThread");
        return;
    }

    if (MH_CreateHook(pFunc, &HookedNtQueryInformationThread, reinterpret_cast<LPVOID*>(&fpNtQueryInformationThread)) != MH_OK) {
        Console::print("Failed to create hook for NtQueryInformationThread");
        return;
    }

    if (MH_EnableHook(pFunc) != MH_OK) {
        Console::print("Failed to enable hook for NtQueryInformationThread");
        return;
    }
}

void DebugPatches::init() {
    Console::initPrint("DebugPatches::init()");
    bypassHwndChecks();
    antiDebuggerSyscalls();
    bypassHiddenThreadCreation();
    bypassThreadHideFromDebugger();
    bypassThreadQueryHideFromDebugger();
    restoreAllPatchedFunctions();
}
