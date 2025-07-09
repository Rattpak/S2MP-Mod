///////////////////////////////////////////
//         Debug Patches
//	Patching some anti-tamper stuff
////////////////////////////////////////////
#include "pch.h"
#include "Arxan.hpp"

#pragma intrinsic(_ReturnAddress)

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
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Restoring debugger-blocked functions...");
#endif // ARXAN_DEBUG_INFO  
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

NTSTATUS WINAPI HookedNtUserBuildHwndList(HDESK hdesk, HWND hwndParent, BOOL fChildren, BOOL fOwner, DWORD dwThreadId, UINT cHwndMax, HWND* phwnd, PUINT pcHwndNeeded) {
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
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Patching HWND Checks");
#endif // ARXAN_DEBUG_INFO

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

using NtClose_t = NTSTATUS(NTAPI*)(HANDLE);
using NtQueryInformationProcess_t = NTSTATUS(NTAPI*)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
NtClose_t fpNtClose = nullptr;
NtQueryInformationProcess_t fpNtQueryInformationProcess = nullptr;

typedef NTSTATUS(NTAPI* PNTQUERYOBJECT)(
    HANDLE Handle,
    OBJECT_INFORMATION_CLASS ObjectInformationClass,
    PVOID ObjectInformation,
    ULONG ObjectInformationLength,
    PULONG ReturnLength
    );

PNTQUERYOBJECT pNtQueryObject = nullptr;


NTSTATUS NTAPI HookedNtClose(HANDLE handle) {
    if (pNtQueryObject) {
        char info[16] = { 0 };
        if (pNtQueryObject(handle, (OBJECT_INFORMATION_CLASS)4, &info, 2, nullptr) >= 0 && (uint64_t)handle != 0x12345) {
            return fpNtClose(handle);
        }
    }
    return STATUS_INVALID_HANDLE;
}
#ifndef ProcessDebugPort
#define ProcessDebugPort 7
#endif 

#ifndef ProcessDebugObjectHandle
#define ProcessDebugObjectHandle 30
#endif 

#ifndef ProcessDebugFlags
#define ProcessDebugFlags 31
#endif 
using fnNtQueryInformationProcess = NTSTATUS(WINAPI*)(
    HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
fnNtQueryInformationProcess oNtQueryInformationProcess = nullptr;
NTSTATUS WINAPI HookedNtQueryInformationProcess(
    HANDLE handle,
    PROCESSINFOCLASS info_class,
    PVOID info,
    ULONG info_length,
    PULONG ret_length)
{
    const NTSTATUS status = oNtQueryInformationProcess(handle, info_class, info, info_length, ret_length);

    if (NT_SUCCESS(status)) {
        if (info_class == ProcessBasicInformation) {
            static DWORD explorer_pid = 0;
            if (!explorer_pid) {
                HWND shell_window = GetShellWindow();
                GetWindowThreadProcessId(shell_window, &explorer_pid);
            }

            reinterpret_cast<PPROCESS_BASIC_INFORMATION>(info)->Reserved3 =
                reinterpret_cast<PVOID>(static_cast<ULONG_PTR>(explorer_pid));
        }
        else if (info_class == ProcessDebugObjectHandle) {
            *reinterpret_cast<HANDLE*>(info) = nullptr;
            return STATUS_PORT_NOT_SET; // 0xC0000353
        }
        else if (info_class == ProcessDebugPort) {
            *reinterpret_cast<HANDLE*>(info) = nullptr;
        }
        else if (info_class == 31 /* ProcessDebugFlags */) {
            *reinterpret_cast<ULONG*>(info) = 1;
        }
    }

    return status;
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
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Patching NtCreateThreadEx");
#endif // ARXAN_DEBUG_INFO

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
NTSTATUS WINAPI HookedNtSetInformationThread(HANDLE ThreadHandle, THREAD_INFORMATION_CLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength) {
    //block attempts to hide thread from debooger
                                      //ThreadHideFromDebugger v
    if (ThreadInformationClass == (THREAD_INFORMATION_CLASS)ThreadHideFromDebugger) {
#ifdef ARXAN_DEBUG_INFO
        Console::labelPrint(DebugPatches::conLabel, "Blocked ThreadHideFromDebugger via NtSetInformationThread");
#endif // ARXAN_DEBUG_INFO     
        return 0; //success
    }

    return fpNtSetInformationThread(ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength);
}

void antiDebuggerSyscalls() {
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Patching CheckRemoteDebuggerPresent");
#endif // ARXAN_DEBUG_INFO

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
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Patching ThreadHideFromDebugger");
#endif // ARXAN_DEBUG_INFO

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

void hookNtClose() {
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Hooking Anti-Debugging APIs");
#endif // ARXAN_DEBUG_INFO


    HMODULE hNtDll = GetModuleHandleA("ntdll.dll");
    if (hNtDll) {
        pNtQueryObject = (PNTQUERYOBJECT)GetProcAddress(hNtDll, "NtQueryObject");
        if (!pNtQueryObject) {
            Console::print("Failed to resolve NtQueryObject");
            return;
        }
        MH_CreateHook(GetProcAddress(hNtDll, "NtClose"), HookedNtClose, reinterpret_cast<LPVOID*>(&fpNtClose));
    }
    MH_EnableHook(MH_ALL_HOOKS);
}

#ifdef ARXAN_LOG_VEH
using AddVEHFunc = PVOID(WINAPI*)(ULONG, PVECTORED_EXCEPTION_HANDLER);
AddVEHFunc fpAddVEH = nullptr;

std::ofstream vehLog("VEH_Log.txt", std::ios::app);

void LogVEHInfo(const std::string& msg) {
    if (vehLog.is_open())
        vehLog << msg << std::endl;
    //Console::print(msg);
}

std::string DumpBytes(void* addr, size_t length = 16) {
    std::ostringstream oss;
    //__try {
    unsigned char* p = reinterpret_cast<unsigned char*>(addr);
    for (size_t i = 0; i < length; ++i) {
        oss << std::setw(2) << std::setfill('0') << std::hex << (int)p[i] << " ";
    }
    // }
     //__except (EXCEPTION_EXECUTE_HANDLER) {
       //  oss << "[unreadable memory]";
    // }
    return oss.str();
}

PVOID WINAPI HookedAddVEH(ULONG First, PVECTORED_EXCEPTION_HANDLER Handler) {
    void* returnAddress = _ReturnAddress();

    std::string handlerBytes = DumpBytes((void*)Handler, 24);
    std::string returnAddrBytes = DumpBytes(returnAddress, 24);

    char buffer[1024];
    snprintf(buffer, sizeof(buffer),
        "\n[VEH Installed]\n"
        "  First: %lu\n"
        "  Handler: 0x%p\n"
        "    Bytes: %s\n"
        "  ReturnAddress: 0x%p\n"
        "    Bytes: %s\n",
        First,
        Handler, handlerBytes.c_str(),
        returnAddress, returnAddrBytes.c_str());

    LogVEHInfo(buffer);

    return fpAddVEH(First, Handler);
}

void hookAddVEH() {
    Console::labelPrint(DebugPatches::conLabel, "Patching AddVectoredExceptionHandler");

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32) {
        Console::print("Failed to get handle for kernel32.dll");
        return;
    }

    FARPROC pFunc = GetProcAddress(hKernel32, "AddVectoredExceptionHandler");
    if (!pFunc) {
        Console::print("Failed to get address of AddVectoredExceptionHandler");
        return;
    }

    if (MH_CreateHook(pFunc, &HookedAddVEH, reinterpret_cast<LPVOID*>(&fpAddVEH)) != MH_OK) {
        Console::print("Failed to create hook for AddVectoredExceptionHandler");
        return;
    }

    if (MH_EnableHook(pFunc) != MH_OK) {
        Console::print("Failed to enable hook for AddVectoredExceptionHandler");
        return;
    }
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Successfully hooked AddVectoredExceptionHandler");
#endif // ARXAN_DEBUG_INFO

}


void DumpKnownAPIs() {
    HMODULE hKernel = GetModuleHandleA("kernel32.dll");
    if (!hKernel) return;

    std::vector<std::string> apis = {
        "GetSystemInfo", "VirtualProtect", "GetTickCount",
        "IsDebuggerPresent", "CreateFileW", "ReadFile"
    };

    LogVEHInfo("\n[API Fingerprints]");
    for (const auto& name : apis) {
        FARPROC addr = GetProcAddress(hKernel, name.c_str());
        if (addr) {
            char line[256];
            snprintf(line, sizeof(line), "  %s = 0x%p", name.c_str(), addr);
            LogVEHInfo(line);
        }
    }
}
#endif

void hide_being_debugged() {
    auto* const peb = PPEB(__readgsqword(0x60));
    peb->BeingDebugged = false;
    *reinterpret_cast<PDWORD>(LPSTR(peb) + 0xBC) &= ~0x70;
}

std::atomic_bool hbdRunning = true;
void startHideDebugLoop() {
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Starting HBD Thread");
#endif // ARXAN_DEBUG_INFO

    std::thread([] {
        while (hbdRunning) {
            hide_being_debugged();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        }).detach();
}


#ifndef ProcessDebugObjectHandle
#define ProcessDebugObjectHandle static_cast<PROCESSINFOCLASS>(0x1e)
#endif

#ifndef ProcessDebugPort
#define ProcessDebugPort static_cast<PROCESSINFOCLASS>(0x7)
#endif

#ifndef ProcessDebugFlags
#define ProcessDebugFlags static_cast<PROCESSINFOCLASS>(0x1f)
#endif

void clearHWBP(void) {
    CONTEXT ctx{};
    ZeroMemory(&ctx, sizeof(CONTEXT));
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    HANDLE thread = GetCurrentThread();
    if (thread != NULL) {
        GetThreadContext(thread, &ctx);
        ctx.Dr0 = 0;
        ctx.Dr1 = 0;
        ctx.Dr2 = 0;
        ctx.Dr3 = 0;
        ctx.Dr6 = 0;
        ctx.Dr7 = 0;
        SetThreadContext(thread, &ctx);
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

void bypassThreadQueryHideFromDebugger() {
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Patching NtQueryInformationThread");
#endif // ARXAN_DEBUG_INFO


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

LONG WINAPI interrupt2dFilter(const LPEXCEPTION_POINTERS info) {
    if (info->ExceptionRecord->ExceptionCode == STATUS_INVALID_HANDLE)
    {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void hookNtQueryInformationProcess() {
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Patching NtQueryInformationProcess");
#endif // ARXAN_DEBUG_INFO
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        Console::print("Failed to get handle for ntdll.dll");
        return;
    }

    FARPROC pFunc = GetProcAddress(hNtdll, "NtQueryInformationProcess");
    if (!pFunc) {
        Console::print("Failed to get address of NtQueryInformationProcess");
        return;
    }

    if (MH_CreateHook(pFunc, &HookedNtQueryInformationProcess, reinterpret_cast<LPVOID*>(&oNtQueryInformationProcess)) != MH_OK) {
        Console::print("Failed to create hook for NtQueryInformationProcess");
        return;
    }

    if (MH_EnableHook(pFunc) != MH_OK) {
        Console::print("Failed to enable hook for NtQueryInformationProcess");
        return;
    }

#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Successfully hooked NtQueryInformationProcess");
#endif // ARXAN_DEBUG_INFO
}

void DebugPatches::init() {
#ifdef DEVELOPMENT_BUILD
    Console::initPrint(std::string(__FUNCTION__));
#endif // DEVELOPMENT_BUILD
    bypassHwndChecks();
    antiDebuggerSyscalls();
    bypassHiddenThreadCreation();
    bypassThreadHideFromDebugger();
    bypassThreadQueryHideFromDebugger();
#ifdef ARXAN_LOG_VEH
    hookAddVEH();
    DumpKnownAPIs();
#endif
    restoreAllPatchedFunctions();
    startHideDebugLoop();
    hookNtQueryInformationProcess();
    hookNtClose();
    clearHWBP();

#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(DebugPatches::conLabel, "Adding VEH");
#endif // ARXAN_DEBUG_INFO
    AddVectoredExceptionHandler(1, interrupt2dFilter); //https://unprotect.it/technique/int-0x2d/
}