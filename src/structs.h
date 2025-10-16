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

//size 0x28
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
    ASSET_TYPE_GFXWORLD = 0x27,
    ASSET_TYPE_LIGHT_DEF = 0x2B,
    ASSET_TYPE_WEAPON = 0x30,
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

enum FF_DIR : __int32
{
    FFD_DEFAULT = 0x0,
    FFD_USER_MAP = 0x1,
};

enum HksError : __int32
{
    HKS_NO_ERROR = 0x0,
    LUA_ERRSYNTAX = 0xFFFFFFFC,
    LUA_ERRFILE = 0xFFFFFFFB,
    LUA_ERRRUN = 0xFFFFFF9C,
    LUA_ERRMEM = 0xFFFFFF38,
    LUA_ERRERR = 0xFFFFFED4,
    HKS_THROWING_ERROR = 0xFFFFFE0C,
    HKS_GC_YIELD = 0x1,
};

enum dvarType_t : int8_t
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
    DVAR_TYPE_COLOR2 = 0x9,
    DVAR_TYPE_BOOL_AGAIN = 0xA, //
    DVAR_TYPE_FLOAT_SPECIAL = 0xB, //
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

struct DvarValueFloatSpecial
{
    float f1; //TODO: figure this out
    float f2; //TODO: figure this out
    float f3; //TODO: figure this out
    float f4; //TODO: figure this out
    float value;
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
    DvarValueFloatSpecial floatSpecial;
};

//TODO: find proper dvar structure
struct dvar_t 
{
    const char* name;
    int flags;
    dvarType_t type; //1 byte
    bool modified; //0x16
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

struct CmdArgs
{
    int nesting;
    LocalClientNum_t localClientNum[8];
    int controllerIndex[8];
    int argc[8];
    const char** argv[8];
};

//WIP
struct gentity_s {

};

//SIZE: 0xA8
struct ComPrimaryLight {
    unsigned char type; //0x0
    unsigned char canUseShadowMap; //0x1
    unsigned char exponent; //0x2

    /*
        UNKNOWN
    */
    unsigned char pad03; //0x3
    unsigned char pad04[0x0C]; //0x04 --> 0x0F


    float color[3]; //0x10 --> 0x18
    float dir[3]; //0x1C --> 0x24
    float up[3]; //0x28 --> 0x30
    float origin[3]; //0x34 --> 0x3C
    float fadeOffset[2]; //0x40 --> 0x44
    float bulbRadius; //0x48
    float bulbLength[3]; //0x4C --> 0x54
    float radius; //0x58
    float cosHalfFovOuter; //0x5C
    float cosHalfFovInner; //0x60

    /*
        UNKNOWN
    */
    unsigned char pad64[0x3C]; //0x64 --> 0x9F

    const char* defName; //0xA0
};

//DOUBLE CHECK THIS
struct cplane_s {
    float normal[3];
    float dist;
    unsigned char type;
    unsigned char pad[3];
};

//DOUBLE CHECK THIS
struct GfxWorldDpvsPlanes {
    int cellCount;
    cplane_s* planes;
    unsigned __int16* nodes;
    unsigned int* sceneEntCellBits;
};

//WIP
struct GfxWorld {

    GfxWorldDpvsPlanes dpvsPlanes; //0x58
    /*
    
    */
    unsigned int* cellCasterBits; //0xC50
};
