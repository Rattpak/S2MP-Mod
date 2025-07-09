/////////////////////////////////////////////////
//         Development Patches
// Temporary patches for testing stuff
/////////////////////////////////////////////////

#include "pch.h"
#include "DevPatches.hpp"
#include "Console.hpp"
#include "FuncPointers.h"
#include "GameUtil.hpp"

#include <xsk/gsc/engine/s2.hpp>
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
    void* target = (void*)(GameUtil::base + 0x6AE0E0);

    if (MH_CreateHook(target, &Cmd_AddCommandInternal_hookfunc, reinterpret_cast<void**>(&oCmd_AddCommandInternal)) != MH_OK) {
        Console::devPrint("ERROR: MH_CreateHook failure in function " + std::string(__FUNCTION__));
        return;
    }

    if (MH_EnableHook(target) != MH_OK) {
        Console::devPrint("ERROR: MH_EnableHook failure in function " + std::string(__FUNCTION__));
        return;
    }

}

static void nlog(const char* fmt, ...)
{
    va_list ap;
    HWND notepad, edit;
    char buf[256];

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    strcat(buf, "");
    notepad = FindWindowA(NULL, ("Untitled - Notepad"));
    if (!notepad)
        notepad = FindWindowA(NULL, ("*Untitled - Notepad"));

    edit = FindWindowExA(notepad, NULL, "EDIT", NULL);
    SendMessageA(edit, EM_REPLACESEL, 0, (LPARAM)buf);
}

typedef XAssetHeader(*DB_FindXAssetHeader_t)(XAssetType type, const char* name, int allowCreateDefault);
static DB_FindXAssetHeader_t DB_FindXAssetHeader = DB_FindXAssetHeader_t(GameUtil::base + 0xFAB20);

typedef void(*ProcessScript_t)(const char* name);
static ProcessScript_t ProcessScript = ProcessScript_t(GameUtil::base + 0x6F6740);


void dumpScript(std::string name, ScriptFile* buffer, int len) {
    std::string filePath = "s2mod/dump/" + name;

    if (buffer == nullptr || buffer->buffer == nullptr) {
        Console::print("Buffer nullptr on script: " + name);
        return;
    }

    std::filesystem::path dirPath = std::filesystem::path(filePath).parent_path();
    if (!std::filesystem::exists(dirPath)) {
        std::filesystem::create_directories(dirPath);
    }

    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) {
        Console::print("File path error for: " + filePath);
        return;
    }

    // Write "GSC\0" header
    const char header[4] = { 'G', 'S', 'C', '\0' };
    outFile.write(header, sizeof(header));

    // Write int values in little-endian order
    auto writeInt = [&](int value) {
        outFile.write(reinterpret_cast<const char*>(&value), sizeof(int));
        };

    writeInt(buffer->compressedLen);
    writeInt(buffer->len);
    writeInt(buffer->bytecodeLen);

    // Write bytecode buffer
    outFile.write(buffer->buffer, len);
    outFile.write(reinterpret_cast<const char*>(buffer->bytecode), buffer->bytecodeLen);

    outFile.close();

    Console::print("Script Dumped to: " + filePath);
}

void* AllocateExecutableMemory(void* addr, SIZE_T size) {
    void* mem = VirtualAlloc(
        addr,                    // Let the system choose the address
        size,                       // Size of allocation
        MEM_COMMIT | MEM_RESERVE,  // Commit and reserve pages
        PAGE_EXECUTE_READWRITE     // Allow read, write, and execute
    );

    if (!mem) {
        // Handle allocation failure
        DWORD err = GetLastError();
    }

    return mem;
}

void* FindXAssetHeader(XAssetType type, const char* name, int allowCreateDefault)
{
    auto result = DB_FindXAssetHeader(type, name, allowCreateDefault);

    if (type == ASSET_TYPE_SCRIPTFILE)
    {
        // dumpScript(name + std::string(".gscbin"), result.script, result.script->compressedLen);
        if ((void*)_ReturnAddress() == (void*)(GameUtil::base + 0x6F675C))
        {
            if (!strcmp(name, "1262"))
            {
                std::ifstream stream("data\\compiled\\s2\\main.gscbin", std::ios::binary | std::ios::ate);
                if (!stream.is_open())
                    return result.data;

                std::streamsize size = stream.tellg();
                stream.seekg(0, std::ios::beg);

                ScriptFile* custom_script = (ScriptFile*)AllocateExecutableMemory(nullptr, sizeof(ScriptFile));
                char* allocMemAddress = (char*)AllocateExecutableMemory(nullptr, size + 1);

                stream.read(allocMemAddress, size);
                stream.seekg(std::ios::beg);

                while (stream.get() != '\0')
                {
                }

                int vars[3] = { 0 };
                stream.read((char*)vars, sizeof(int) * 3);

                custom_script->compressedLen = vars[0];
                custom_script->len = vars[1];
                custom_script->bytecodeLen = vars[2];
                custom_script->buffer = allocMemAddress + (int)stream.tellg();
                custom_script->bytecode = (unsigned char*)(allocMemAddress + vars[0] + (int)stream.tellg());

                return custom_script;
            }
        }
    }

    return result.data;

}

void LoadScripts(const char* name)
{
    if(!strcmp(name, "1262"))
    {
        ScriptFile* script = DB_FindXAssetHeader(ASSET_TYPE_SCRIPTFILE, name, 1).script;
        std::ifstream stream("data\\compiled\\s2\\main.gscbin", std::ios::binary | std::ios::ate);
        if (!stream.is_open())
            return ProcessScript(name);

        std::streamsize size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        auto Load_scriptFileAsset = reinterpret_cast<void* (*)(ScriptFile**)>(GameUtil::base + 0x13CC70);
        char* allocMemAddress = (char*)Load_scriptFileAsset(&script);

        stream.read(allocMemAddress, size);
        stream.seekg(0, std::ios::beg);

        int dummy = { 0 };
        stream.read((char*)&dummy, sizeof(int));

        int vars[3] = { 0 };
        stream.read((char*)vars, sizeof(int) * 3);

        script->compressedLen = vars[0];
        script->len = vars[1];
        script->bytecodeLen = vars[2];
        script->buffer = allocMemAddress + (int)stream.tellg();
        memcpy(script->bytecode, allocMemAddress + vars[0] + (int)stream.tellg(), vars[2]);
    }

    ProcessScript(name);
}

typedef void(*Mark_ScriptFileAsset_t)(ScriptFile*, int);
Mark_ScriptFileAsset_t Mark_ScriptFileAsset = Mark_ScriptFileAsset_t(GameUtil::base + 0x140F00);
bool hasdone = false;
void ScriptFileAsset(ScriptFile* script, int inuse)
{
    if (script)
    {
        if (!strcmp(script->name, "1262"))
        {
            Console::devPrint("FOUND FILE!");

            std::ifstream stream("data\\compiled\\s2\\main.gscbin", std::ios::binary | std::ios::ate);
            if (!stream.is_open())
                return Mark_ScriptFileAsset(script, inuse);

            std::streamsize size = stream.tellg();
            stream.seekg(0, std::ios::beg);

            ScriptFile* custom_script = (ScriptFile*)AllocateExecutableMemory(nullptr, sizeof(ScriptFile));
            char* allocMemAddress = (char*)AllocateExecutableMemory(nullptr, size + 1);

            Console::devPrint("Created Allocated Memory!");
            stream.read(allocMemAddress, size);
            stream.seekg(std::ios::beg);

            while (stream.get() != '\0')
            {
            }

            int vars[3] = { 0 };
            stream.read((char*)vars, sizeof(int) * 3);

            custom_script->compressedLen = vars[0];
            custom_script->len = vars[1];
            custom_script->bytecodeLen = vars[2];
            custom_script->buffer = allocMemAddress + (int)stream.tellg();
            custom_script->bytecode = (unsigned char*)(allocMemAddress + vars[0] + (int)stream.tellg());
            Console::devPrint("Wrote all vars!");
            return Mark_ScriptFileAsset(custom_script, 1);
        }
    }

    Mark_ScriptFileAsset(script, inuse);
}

void DevPatches::init() {
    Console::initPrint("DevPatches::init()");
    HookNtCreateUserProcess();


    if (MH_CreateHook((void*)(GameUtil::base + 0xFAB20), &FindXAssetHeader, reinterpret_cast<void**>(&DB_FindXAssetHeader)) != MH_OK) {
        Console::devPrint("ERROR: MH_CreateHook failure in function " + std::string(__FUNCTION__));
        return;
    }
    if (MH_EnableHook((void*)(GameUtil::base + 0xFAB20)) != MH_OK) {
        Console::devPrint("ERROR: MH_EnableHook failure in function " + std::string(__FUNCTION__));
        return;
    }


    //if (MH_CreateHook((void*)(GameUtil::base + 0x6F6740), &LoadScripts, reinterpret_cast<void**>(&ProcessScript)) != MH_OK) {
    //    Console::devPrint("ERROR: MH_CreateHook failure in function " + std::string(__FUNCTION__));
    //    return;
    //}
    //if (MH_EnableHook((void*)(GameUtil::base + 0x6F6740)) != MH_OK) {
    //    Console::devPrint("ERROR: MH_EnableHook failure in function " + std::string(__FUNCTION__));
    //    return;
    //}


    //if (MH_CreateHook((void*)(GameUtil::base + 0x140F00), &ScriptFileAsset, reinterpret_cast<void**>(&Mark_ScriptFileAsset)) != MH_OK) {
    //    Console::devPrint("ERROR: MH_CreateHook failure in function " + std::string(__FUNCTION__));
    //    return;
    //}
    //if (MH_EnableHook((void*)(GameUtil::base + 0x140F00)) != MH_OK) {
    //    Console::devPrint("ERROR: MH_EnableHook failure in function " + std::string(__FUNCTION__));
    //    return;
    //}

    //Hook_Cmd_AddCommandInternal();
}