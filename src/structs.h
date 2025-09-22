//////////////////////////
//    structs.h
//	Engine Structs
///////////////////////////
#pragma once
#include <cstdint>

enum DvarFlags : std::uint32_t
{
    DVAR_FLAG_NONE = 0,
    DVAR_FLAG_SAVED = 0x1,
    DVAR_FLAG_LATCHED = 0x2,
    DVAR_FLAG_CHEAT = 0x4,
};

struct CmdText
{
    unsigned __int8* data;
    int maxsize;
    int cmdsize;
};

enum LocalClientNum_t : __int32
{
    LOCAL_CLIENT_INVALID = 0xFFFFFFFF,
    LOCAL_CLIENT_0 = 0x0,
    LOCAL_CLIENT_LAST = 0x0,
    LOCAL_CLIENT_COUNT = 0x1,
};

enum errorParm_t
{
    ERR_FATAL = 0x0,
    ERR_DROP = 0x1,
    ERR_SERVERDISCONNECT = 0x2,
    ERR_DISCONNECT = 0x3,
    ERR_SCRIPT = 0x4,
    ERR_SCRIPT_DROP = 0x5,
    ERR_LOCALIZATION = 0x6,
    ERR_MAPLOADERRORSUMMARY = 0x7,
};
struct XZoneInfo
{
    const char* name;
    int allocFlags;
    int freeFlags;
};
struct DvarLimits_integer
{
    int min;
    int max;
};

struct DvarLimits_value
{
    float min;
    float max;
};

union DvarLimits
{
    DvarLimits_integer integer;
    DvarLimits_value floats;
};

//temp
struct material_t {
    const char* name;
};

//temp
struct glyph_t {
    unsigned short letter;
    char x0;
    char y0;
    char dx;
    char pixelWidth;
    char pixelHeight;
    float s0;
    float t0;
    float s1;
    float t1;
};

//temp
struct font_t {
    const char* fontName;
    int pixelHeight;
    int glyphCount;
    material_t* material;
    material_t* glowMaterial;
    glyph_t* glyphs;
};

//temp
struct cmd_function_t {
    cmd_function_t* next;
    const char* name;
    void(__cdecl* func)(void);
};

struct LuaFile
{
    const char* name;
    int len;
    unsigned __int8 strippingType;
    const unsigned __int8* buffer;
};

struct ScriptFile
{
    const char* name;
    int compressedLen;
    int len;
    int bytecodeLen;
    const char* buffer;
    char* bytecode;
};

struct RawFile
{
    const char* name;
    int compressedLen;
    int len;
    const char* buffer;
};

enum XAssetType {
    ASSET_TYPE_XMODEL = 0xA,
    ASSET_TYPE_MATERIAL = 0xD,
    ASSET_TYPE_RAWFILE = 0x39,
    ASSET_TYPE_SCRIPTFILE = 0x3A,
    ASSET_TYPE_STRINGTABLE = 0x3B,
    ASSET_TYPE_LUA_FILE = 0x45,
    ASSET_TYPE_FONT = 0x4E,
};

union XAssetHeader
{
    void* data;
    struct ScriptFile* script;
    struct RawFile* rawfile;
};

struct cmd_function_s
{
    cmd_function_s* next;
    const char* name;
    void(__fastcall* function)();
};

struct CmdArgs
{
    int nesting;
    int localClientNum[8];
    int controllerIndex[8];
    int argc[8];
    const char** argv[8];
};

enum dvarType_t : __int32
{
    DVAR_TYPE_BOOL = 0x0,
    DVAR_TYPE_FLOAT = 0x1,
    DVAR_TYPE_FLOAT_2 = 0x2,
    DVAR_TYPE_FLOAT_3 = 0x3,
    DVAR_TYPE_FLOAT_4 = 0x4,
    DVAR_TYPE_INT = 0x5,
    DVAR_TYPE_ENUM = 0x6,
    DVAR_TYPE_STRING = 0x7,
    DVAR_TYPE_COLOR = 0x8,
    DVAR_TYPE_INT64 = 0x9,
    DVAR_TYPE_LINEAR_COLOR_RGB = 0xA,
    DVAR_TYPE_COLOR_XYZ = 0xB,
    DVAR_TYPE_COUNT = 0xC,
};

struct DvarValueBool
{
    bool enabled;
    char pad[3];
    int hashedValue;
};

struct DvarValueInt
{
    int integer;
    int hashedValue;
};

struct DvarValueEnum
{
    int defaultIndex;
    int hashedValue;
};

union DvarValue
{
    bool enabled;
    int integer;
    unsigned int unsignedInt;
    float value;
    float vector[4];
    const char* string;
    unsigned __int8 color[4];

    DvarValueBool boolean_;
    DvarValueInt integer_;
    DvarValueEnum enumeration_;
};

//TODO: find proper dvar structure
struct dvar_t 
{
    const char* name;
    int flags;
    dvarType_t type;
    DvarValue current;
    //more stuff
};

//making this from scratch
struct playerState_s
{
    int commandTime;
    int pm_type;
    int pm_time;
    int pm_flags;
    int otherFlags;
    int linkFlags;
    int bobCycle;
    float origin[3];
    float velocity[3];
    //more...
};