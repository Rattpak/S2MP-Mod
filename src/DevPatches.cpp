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
#include <utils/compression.h>
#include <utils/string.h>

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

typedef XAssetHeader(*DB_FindXAssetHeader_t)(XAssetType type, const char* name, int allowCreateDefault);
static DB_FindXAssetHeader_t DB_FindXAssetHeader = DB_FindXAssetHeader_t(0xFAB20_b);

typedef void(*ProcessScript_t)(const char* name);
static ProcessScript_t ProcessScript = ProcessScript_t(0x6F6740_b);


void dumpScript(std::string name, ScriptFile* buffer, int len) {
    std::string filePath = "s2mp-mod/dump/" + name;

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

utils::memory::allocator scriptfile_allocator;
std::unordered_map<std::string, ScriptFile*> loaded_scripts;

std::unordered_map<std::string, std::uint32_t> main_handles;
std::unordered_map<std::string, std::uint32_t> init_handles;

char* script_mem_buf = nullptr;
std::uint64_t script_mem_buf_size = 0;

static std::unique_ptr<xsk::gsc::s2::context> gsc_ctx = std::make_unique<xsk::gsc::s2::context>(xsk::gsc::instance::server);

struct
{
    char* buf = nullptr;
    char* pos = nullptr;
    const std::uint64_t size = 0x100000i64;
} script_memory;

char* allocate_buffer(size_t size)
{
    if (script_memory.buf == nullptr)
    {
        script_memory.buf = script_mem_buf;
        script_memory.pos = script_memory.buf;
    }

    if (script_memory.pos + size > script_memory.buf + script_memory.size)
    {
        Console::Print(Console::error, "Out of custom script memory");
    }

    const auto pos = script_memory.pos;
    script_memory.pos += size;
    return pos;
}

enum XFileBlock
{
    XFILE_BLOCK_TEMP_ADDITIONAL = 0x0,
    XFILE_BLOCK_TEMP = 0x1,
    XFILE_BLOCK_TEMP_PRELOAD = 0x2,
    XFILE_BLOCK_CALLBACK = 0x3,
    XFILE_BLOCK_RUNTIME = 0x4,
    XFILE_BLOCK_RUNTIME_VIDEO = 0x5,
    XFILE_BLOCK_CACHED_VIDEO = 0x6,
    XFILE_BLOCK_PHYSICAL = 0x7,
    XFILE_BLOCK_VIRTUAL = 0x8,
    XFILE_BLOCK_SCRIPT = 0x9,
    MAX_XFILE_COUNT = 0xA,
};

struct XBlock
{
    char* data;
    unsigned __int64 size;
};

struct XZoneMemory
{
    XBlock blocks[7];
};

utils::hook::detour DB_AllocXZoneMemoryInternal;
bool patch = false;
void db_alloc_x_zone_memory_internal_stub(unsigned __int64* blockSize, const char* filename, XZoneMemory* zoneMem, unsigned int type)
{
    Console::Print(Console::info, "File '%s' found with type '%i'\n", filename, type);

    if (!_stricmp(filename, "common_core_mp") && type == 2)
    {
        patch = true;
        Console::Print(Console::info, "patching memory for '%s'\n", filename);
    }

    if (patch)
    {
        blockSize[XFILE_BLOCK_SCRIPT] += script_memory.size;
    }

    DB_AllocXZoneMemoryInternal.invoke<void>(blockSize, filename, zoneMem, type);

    if (patch)
    {
        blockSize[XFILE_BLOCK_SCRIPT] -= script_memory.size;
        script_mem_buf = zoneMem->blocks[XFILE_BLOCK_SCRIPT].data + blockSize[XFILE_BLOCK_SCRIPT];
        script_mem_buf_size = script_memory.size;
    }
}

bool file_exists(const std::string& file)
{
    return std::ifstream(file).good();
}

bool read_file(const std::string& file, std::string* data)
{
    if (!data) return false;
    data->clear();

    if (file_exists(file))
    {
        std::ifstream stream(file, std::ios::binary);
        if (!stream.is_open()) return false;

        stream.seekg(0, std::ios::end);
        const std::streamsize size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        if (size > -1)
        {
            data->resize(static_cast<uint32_t>(size));
            stream.read(const_cast<char*>(data->data()), size);
            stream.close();
            return true;
        }
    }

    return false;
}

typedef void (*DB_GetRawBuffer_t)(RawFile* rawfile, char* buf, int size);
static DB_GetRawBuffer_t DB_GetRawBuffer = DB_GetRawBuffer_t(0xFC190_b);

typedef bool (*DB_IsXAssetDefault_t)(XAssetType type, const char* name);
static DB_IsXAssetDefault_t DB_IsXAssetDefault = DB_IsXAssetDefault_t(0xFD810_b);

typedef bool (*DB_XAssetExists_t)(XAssetType type, const char* name);
static DB_XAssetExists_t DB_XAssetExists = DB_XAssetExists_t(0x1072B0_b);

typedef int64_t (*DB_GetRawFileLen_t)(RawFile* rawfile);
static DB_GetRawFileLen_t DB_GetRawFileLen = DB_GetRawFileLen_t(0xFC300_b);

bool read_raw_script_file(const std::string& name, std::string* data)
{
    if (read_file(name, data))
    {
        return true;
    }

    const auto* name_str = name.data();
    if (DB_XAssetExists(ASSET_TYPE_RAWFILE, name_str) &&
        !DB_IsXAssetDefault(ASSET_TYPE_RAWFILE, name_str))
    {
        const auto asset = DB_FindXAssetHeader(ASSET_TYPE_RAWFILE, name_str, false);
        const auto len = DB_GetRawFileLen(asset.rawfile);
        data->resize(len);
        DB_GetRawBuffer(asset.rawfile, data->data(), len);
        if (len > 0)
        {
            data->pop_back();
        }

        return true;
    }

    return false;
}

ScriptFile* load_custom_script(const char* file_name, const std::string& real_name)
{
    if (const auto itr = loaded_scripts.find(file_name); itr != loaded_scripts.end())
    {
        return itr->second;
    }

    if (!patch)
    {
        return nullptr;
    }

    std::string source_buffer{};
    if (!read_raw_script_file(real_name + ".gsc", &source_buffer) || source_buffer.empty())
    {
        return nullptr;
    }

    // filter out "GSC rawfiles" that were used for development usage and are not meant for us.
    // each "GSC rawfile" has a ScriptFile counterpart to be used instead
    if (DB_XAssetExists(ASSET_TYPE_SCRIPTFILE, file_name) &&
        !DB_IsXAssetDefault(ASSET_TYPE_SCRIPTFILE, file_name))
    {
        if (real_name.starts_with(string::va("scripts/mp/maps/"))
            && (real_name.ends_with("_fx") || real_name.ends_with("_sound")))
        {
            Console::Print(Console::info,"Refusing to compile rawfile '%s'\n", real_name.data());
            return DB_FindXAssetHeader(ASSET_TYPE_SCRIPTFILE, file_name, false).script;
        }
    }

   Console::Print(Console::info, "Loading custom gsc '%s.gsc'", real_name.data());

    try
    {
        auto& compiler = gsc_ctx->compiler();
        auto& assembler = gsc_ctx->assembler();

        std::vector<std::uint8_t> data;
        data.assign(source_buffer.begin(), source_buffer.end());

        const auto assembly_ptr = compiler.compile(real_name, data);    
        const auto& assembler_result = assembler.assemble(*assembly_ptr);

        const auto script_file_ptr = static_cast<ScriptFile*>(scriptfile_allocator.allocate(sizeof(ScriptFile)));
        script_file_ptr->name = file_name;

        script_file_ptr->len = static_cast<int>(std::get<1>(assembler_result).size);
        script_file_ptr->bytecodeLen = static_cast<int>(std::get<0>(assembler_result).size);

        const auto stack_size = static_cast<std::uint32_t>(std::get<1>(assembler_result).size + 1);
        const auto byte_code_size = static_cast<std::uint32_t>(std::get<0>(assembler_result).size + 1);

        script_file_ptr->buffer = static_cast<char*>(scriptfile_allocator.allocate(stack_size));
        std::memcpy(const_cast<char*>(script_file_ptr->buffer), std::get<1>(assembler_result).data, std::get<1>(assembler_result).size);

        script_file_ptr->bytecode = allocate_buffer(byte_code_size);
        std::memcpy(script_file_ptr->bytecode, std::get<0>(assembler_result).data, std::get<0>(assembler_result).size);

        script_file_ptr->compressedLen = 0;

        loaded_scripts[file_name] = script_file_ptr;

        Console::Print(Console::info, "Loaded custom gsc '%s.gsc'", real_name.data());

        return script_file_ptr;
    }
    catch (const std::exception& e)
    {
        Console::Print(Console::error, "*********** script compile error *************\n");
        Console::Print(Console::error, "failed to compile '%s':\n%s", real_name.data(), e.what());
        Console::Print(Console::error, "**********************************************\n");
        return nullptr;
    }
}

std::string get_script_file_name(const std::string& name)
{
    const auto id = gsc_ctx->token_id(name);
    if (!id)
    {
        return name;
    }

    return std::to_string(id);
}

std::pair<xsk::gsc::buffer, std::vector<std::uint8_t>> read_compiled_script_file(const std::string& name, const std::string& real_name)
{
    const auto* script_file = DB_FindXAssetHeader(ASSET_TYPE_SCRIPTFILE, name.data(), false).script;
    if (script_file == nullptr)
    {
        throw std::runtime_error(std::format("Could not load scriptfile '{}'", real_name));
    }

    Console::Print(Console::info, "Decompiling scriptfile '%s'\n", real_name.data());

    const auto len = script_file->compressedLen;
    const std::string stack{ script_file->buffer, static_cast<std::uint32_t>(len) };

    const auto decompressed_stack = compression::decompress(stack);

    std::vector<std::uint8_t> stack_data;
    stack_data.assign(decompressed_stack.begin(), decompressed_stack.end());

    return { {reinterpret_cast<std::uint8_t*>(script_file->bytecode), static_cast<std::uint32_t>(script_file->bytecodeLen)}, stack_data };
}

void init_compiler()
{
    gsc_ctx->init(xsk::gsc::build::prod, []([[maybe_unused]] auto const* ctx, const auto& included_path) -> std::pair<xsk::gsc::buffer, std::vector<std::uint8_t>>
        {
            const auto script_name = std::filesystem::path(included_path).replace_extension().string();

            std::string file_buffer;
            if (!read_raw_script_file(included_path, &file_buffer) || file_buffer.empty())
            {
                const auto name = get_script_file_name(script_name);
                if (DB_XAssetExists(ASSET_TYPE_SCRIPTFILE, name.data()))
                {
                    return read_compiled_script_file(name, script_name);
                }

                throw std::runtime_error(std::format("Could not load gsc file '{}'", script_name));
            }

            std::vector<std::uint8_t> script_data;
            script_data.assign(file_buffer.begin(), file_buffer.end());

            return { {}, script_data };
        });
}

ScriptFile* find_script(XAssetType type, const char* name, int allow_create_default)
{
    std::string real_name = name;
    const auto id = static_cast<std::uint16_t>(std::atoi(name));
    if (id)
    {
        real_name = gsc_ctx->token_name(id);
    }

    auto* script = load_custom_script(name, real_name);
    if (script)
    {
        return script;
    }

    return DB_FindXAssetHeader(type, name, allow_create_default).script;
}

utils::hook::detour DB_FindXAssetHeader_hook;
XAssetHeader FindXAssetHeader(XAssetType type, const char* name, int allowCreateDefault)
{
    auto result = DB_FindXAssetHeader(type, name, allowCreateDefault);

#ifdef DUMP_SCRIPTS
    if (type == ASSET_TYPE_SCRIPTFILE)
    {
        dumpScript(name + std::string(".gscbin"), result.script, result.script->compressedLen);
    }
#endif

    return result;
}

utils::hook::detour Scr_BeginLoadScripts;
utils::hook::detour Scr_EndLoadScripts;

bool directory_exists(const std::string& directory)
{
    return std::filesystem::is_directory(directory);
}

std::vector<std::string> list_files(const std::string& directory)
{
    std::vector<std::string> files;

    for (auto& file : std::filesystem::directory_iterator(directory))
    {
        files.push_back(file.path().generic_string());
    }

    return files;
}

typedef bool(*Scr_LoadScript_t)(const char* name);
static Scr_LoadScript_t Scr_LoadScript = Scr_LoadScript_t(0x6ECC90_b);

typedef bool(*Scr_GetFunctionHandle_t)(const char* name, unsigned int handle);
static Scr_GetFunctionHandle_t Scr_GetFunctionHandle = Scr_GetFunctionHandle_t(0x6ECB30_b);

void load_script(const std::string& name)
{
    if (!Scr_LoadScript(name.data()))
    {
        return;
    }

    const auto main_handle = Scr_GetFunctionHandle(name.data(), gsc_ctx->token_id("main"));
    const auto init_handle = Scr_GetFunctionHandle(name.data(), gsc_ctx->token_id("init"));

    if (main_handle)
    {
        Console::Print(Console::info, "Loaded '%s::main'\n", name.data());
        main_handles[name] = main_handle;
    }

    if (init_handle)
    {
        Console::Print(Console::info, "Loaded '%s::init'\n", name.data());
        init_handles[name] = init_handle;
    }
}

void load_scripts(const std::filesystem::path& root_dir, const std::filesystem::path& subfolder)
{
    std::filesystem::path script_dir = root_dir / subfolder;
    if (!directory_exists(script_dir.generic_string()))
    {
        return;
    }

    const auto scripts = list_files(script_dir.generic_string());
    for (const auto& script : scripts)
    {
        if (!script.ends_with(".gsc"))
        {
            continue;
        }

        Console::Print(Console::info, "gsc file found '%s'", script);

        //std::filesystem::path path(script);
        //const auto relative = path.lexically_relative(root_dir).generic_string();
        //const auto base_name = relative.substr(0, relative.size() - 4);

        //load_script(base_name);
    }
}

void load_scripts()
{
    if (patch)
    {
    }

    load_scripts("s2mp-mod", "custom_scripts/");
}

void scr_begin_load_scripts(int a1)
{
    // start the compiler
    init_compiler();

    Scr_BeginLoadScripts.invoke<void>(a1);

    // load scripts
    load_scripts();
}

void scr_end_load_scripts(int a1)
{
    // cleanup the compiler
    gsc_ctx->cleanup();

    Scr_EndLoadScripts.invoke<void>(a1);
}

void db_get_raw_buffer_stub(RawFile* rawfile, char* buf, const int size)
{
    if (rawfile->len > 0 && rawfile->compressedLen == 0)
    {
        std::memset(buf, 0, size);
        std::memcpy(buf, rawfile->buffer, std::min(rawfile->len, size));
        return;
    }

    DB_GetRawBuffer(rawfile, buf, size);
}

int db_is_x_asset_default(XAssetType type, const char* name)
{
    if (loaded_scripts.contains(name))
    {
        return 0;
    }

    return DB_IsXAssetDefault(type, name);
}

typedef void(*RemoveRefToObject_t)(unsigned int id);
static RemoveRefToObject_t RemoveRefToObject = RemoveRefToObject_t(0x6F76C0_b);

typedef unsigned __int16(*Scr_ExecThread_t)(int handle, unsigned int paramcount);
static Scr_ExecThread_t Scr_ExecThread = Scr_ExecThread_t(0x6F75E0_b);

utils::hook::detour g_load_structs_hook;
utils::hook::detour scr_load_level_hook;
void g_load_structs_stub(float a1)
{
    for (auto& function_handle : main_handles)
    {
        Console::Print(Console::info, "Executing '%s::main'\n", function_handle.first.data());
        RemoveRefToObject(Scr_ExecThread(function_handle.second, 0));
    }

    g_load_structs_hook.invoke<void>(a1);
}

void scr_load_level_stub()
{
    for (auto& function_handle : init_handles)
    {
        Console::Print(Console::info, "Executing '%s::init'\n", function_handle.first.data());
        RemoveRefToObject(Scr_ExecThread(function_handle.second, 0));
    }

    scr_load_level_hook.invoke<void>();
}

void DevPatches::init() 
{
    Console::initPrint("DevPatches::init()");
    HookNtCreateUserProcess();

    // Dump Scripts
    DB_FindXAssetHeader_hook.create(DB_FindXAssetHeader, FindXAssetHeader);

    // Allocate Script Memory
    DB_AllocXZoneMemoryInternal.create(0x53AE30_b, db_alloc_x_zone_memory_internal_stub);

    // Load our scripts with an uncompressed stack
    //utils::hook::call(0x6F67C0_b, db_get_raw_buffer_stub);

    // start compiler, clean up compiler, load scripts
    Scr_BeginLoadScripts.create(0x6EC8C0_b, scr_begin_load_scripts);
    // Scr_EndLoadScripts.create(0x6EC9F0_b, scr_end_load_scripts);

    //// load our scripts
    //utils::hook::call(0x6F6757_b, find_script);
    //utils::hook::call(0x6F6767_b, db_is_x_asset_default);

    //// execute main handle
    //g_load_structs_hook.create(0x61AFE0_b, g_load_structs_stub);

    //// execute init handle
    //scr_load_level_hook.create(0x61B490_b, scr_load_level_stub);
}