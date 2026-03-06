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

struct StringTableCell
{
	const char* string;
	int hash;
};

struct StringTable
{
	const char* name;
	int columnCount;
	int rowCount;
	StringTableCell* values;
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

struct LocalizeEntry
{
    const char* value;
    const char* name;
};

enum XAssetType {
	ASSET_TYPE_PHYSPRESET = 0x0,
	ASSET_TYPE_SND_PHYSPRESET = 0x1,
	ASSET_TYPE_SND_MUSICSET = 0x2,
	ASSET_TYPE_PHYSCOLLMAP = 0x3,
	ASSET_TYPE_PHYSWATERPRESET = 0x4,
	ASSET_TYPE_PHYSWORLDMAP = 0x5,
	ASSET_TYPE_PHYSCONSTRAINT = 0x6,
	ASSET_TYPE_XANIMPARTS = 0x7,
	ASSET_TYPE_XSURFSHARED = 0x8,
	ASSET_TYPE_XMODEL_SURFS = 0x9,
    ASSET_TYPE_XMODEL = 0xA,
    ASSET_TYPE_XMODELBASE = 0xB,
    ASSET_TYPE_MAYHEM = 0xC,
    ASSET_TYPE_MATERIAL = 0xD,
    ASSET_TYPE_COMPUTESHADER = 0xE,
    ASSET_TYPE_VERTEXSHADER = 0xF,
    ASSET_TYPE_HULLSHADER = 0x10,
    ASSET_TYPE_DOMAINSHADER = 0x11,
    ASSET_TYPE_PIXELSHADER = 0x12,
    ASSET_TYPE_VERTEXDECL = 0x13,
	ASSET_TYPE_TECHNIQUE_SET = 0x14,
	ASSET_TYPE_IMAGE = 0x15,
	ASSET_TYPE_SOUND = 0x16,
	ASSET_TYPE_SOUND_SUBMIX = 0x17,
	ASSET_TYPE_SOUND_CURVE = 0x18,
	ASSET_TYPE_DIST_CURVE = 0x19,
	ASSET_TYPE_REVERB_CURVE = 0x1A,
	ASSET_TYPE_SOUND_CONTEXT = 0x1B,
	ASSET_TYPE_ALIAS_PARAMETER_MODIFIER = 0x1C,
	ASSET_TYPE_ALIAS_COMBAT_CONE = 0x1D,
	ASSET_TYPE_LOADED_SOUND = 0x1E,
	ASSET_TYPE_CLIPMAP = 0x1F,
	ASSET_TYPE_COMWORLD = 0x20,
	ASSET_TYPE_GLASSWORLD = 0x21,
	ASSET_TYPE_PATHDATA = 0x22,
	ASSET_TYPE_NAVMESH = 0x23,
	ASSET_TYPE_VEHICLE_TRACK = 0x24,
	ASSET_TYPE_MAP_ENTS = 0x25,
	ASSET_TYPE_FX_MAP = 0x26,
	ASSET_TYPE_GFXWORLD = 0x27,
	ASSET_TYPE_GFXWORLD_TRANS = 0x28,
	ASSET_TYPE_CLIPMAP_TRANS = 0x29,
	ASSET_TYPE_IESPROFILE = 0x2A,
    ASSET_TYPE_LIGHT_DEF = 0x2B,
	ASSET_TYPE_UI_MAP = 0x2C,
	ASSET_TYPE_ANIMCLASS = 0x2D,
	ASSET_TYPE_LOCALIZE_ENTRY = 0x2E,
	ASSET_TYPE_ATTACHMENT = 0x2F,
    ASSET_TYPE_WEAPON = 0x30,
	ASSET_TYPE_SNDDRIVER_GLOBALS = 0x31,
	ASSET_TYPE_FX = 0x32,
	ASSET_TYPE_IMPACT_FX = 0x33,
	ASSET_TYPE_SURFACE_FX = 0x34,
	ASSET_TYPE_AITYPE = 0x35,
	ASSET_TYPE_MPTYPE = 0x36,
	ASSET_TYPE_CHARACTER = 0x37,
	ASSET_TYPE_XMODELALIAS = 0x38,
    ASSET_TYPE_RAWFILE = 0x39,
    ASSET_TYPE_SCRIPTFILE = 0x3A,
    ASSET_TYPE_STRINGTABLE = 0x3B,
	ASSET_TYPE_LEADERBOARD = 0x3C,
	ASSET_TYPE_VIRTUAL_LEADERBOARD = 0x3D,
	ASSET_TYPE_STRUCTURED_DATA_DEF = 0x3E,
	ASSET_TYPE_DDL = 0x3F,
	ASSET_TYPE_TRACER = 0x40,
	ASSET_TYPE_VEHICLE = 0x41,
	ASSET_TYPE_ADDON_MAP_ENTS = 0x42,
	ASSET_TYPE_NET_CONST_STRINGS = 0x43,
	ASSET_TYPE_REVERB_PRESET = 0x44,
    ASSET_TYPE_LUA_FILE = 0x45,
	ASSET_TYPE_SCRIPTABLE = 0x46,
	ASSET_TYPE_EQUIPMENT_SND_TABLE = 0x47,
	ASSET_TYPE_VECTORFIELD = 0x48,
	ASSET_TYPE_PARTICLE_SIM_ANIMATION = 0x49,
	ASSET_TYPE_LASER = 0x4A,
	ASSET_TYPE_BEAM = 0x4B,
	ASSET_TYPE_SKELETON_SCRIPT = 0x4C,
	ASSET_TYPE_CLUT = 0x4D,
    ASSET_TYPE_FONT = 0x4E,
    ASSET_TYPE_SPLINE = 0x4F,
    ASSET_TYPE_PHYS_CLOTH_TUNING = 0x50,
	ASSET_TYPE_DLOGSCHEMA = 0x51,
	ASSET_TYPE_DLOGROUTES = 0x52,
};

//taken from s1
enum GfxWarningType
{
    R_WARN_FRONTEND_ENT_LIMIT = 0,
    R_WARN_KNOWN_MODELS = 1,
    R_WARN_KNOWN_DOBJ = 2,
    R_WARN_KNOWN_BRUSH = 3,
    R_WARN_KNOWN_PER_CLIENT_MODELS = 4,
    R_WARN_KNOWN_SPECIAL_MODELS = 5,
    R_WARN_MODEL_LIGHT_CACHE = 6,
    R_WARN_SCENE_ENTITIES = 7,
    R_WARN_MAX_SKINNED_CACHE_VERTICES = 8,
    R_WARN_MAX_SCENE_SURFS_SIZE = 9,
    R_WARN_MAX_SURF_BUF = 10,
    R_WARN_PORTAL_PLANES = 11,
    R_WARN_MAX_CLOUDS = 12,
    R_WARN_MAX_DLIGHTS = 13,
    R_WARN_MAX_SPOTLIGHTS = 14,
    R_WARN_DLIGHT_SMODEL_LIMIT = 15,
    R_WARN_SMODEL_LIGHTING = 16,
    R_WARN_SMODEL_VIS_DATA_LIMIT = 17,
    R_WARN_SMODEL_SURF_LIMIT = 18,
    R_WARN_SMODEL_SURF_DELAY_LIMIT = 19,
    R_WARN_MARK_SMODEL_COLLIDED_LIMIT = 20,
    R_WARN_MARK_WORLD_BRUSH_LIMIT = 21,
    R_WARN_BSPSURF_DATA_LIMIT = 22,
    R_WARN_BSPSURF_OMNI_LIGHT_LIMIT = 23,
    R_WARN_BSPSURF_SPOT_LIGHT_LIMIT = 24,
    R_WARN_MAX_DRAWSURFS = 25,
    R_WARN_GFX_CODE_EMISSIVE_SURF_LIMIT = 26,
    R_WARN_GFX_CODE_TRANS_SURF_LIMIT = 27,
    R_WARN_GFX_GLASS_SURF_LIMIT = 28,
    R_WARN_GFX_MARK_SURF_LIMIT = 29,
    R_WARN_GFX_SPARK_SURF_LIMIT = 30,
    R_WARN_MAX_SCENE_DRAWSURFS = 31,
    R_WARN_MAX_FX_DRAWSURFS = 32,
    R_WARN_NONEMISSIVE_FX_MATERIAL = 33,
    R_WARN_NONLIT_MARK_MATERIAL = 34,
    R_WARN_CMDBUF_OVERFLOW = 35,
    R_WARN_MISSING_DECL_NONDEBUG = 36,
    R_WARN_MAX_DYNENT_REFS = 37,
    R_WARN_MAX_SCENE_DOBJ_REFS = 38,
    R_WARN_MAX_SCENE_MODEL_REFS = 39,
    R_WARN_MAX_SCENE_BRUSH_REFS = 40,
    R_WARN_MAX_CODE_EMISSIVE_INDS = 41,
    R_WARN_MAX_CODE_EMISSIVE_VERTS = 42,
    R_WARN_MAX_CODE_EMISSIVE_ARGS = 43,
    R_WARN_MAX_CODE_TRANS_INDS = 44,
    R_WARN_MAX_CODE_TRANS_VERTS = 45,
    R_WARN_MAX_CODE_TRANS_ARGS = 46,
    R_WARN_MAX_GLASS_INDS = 47,
    R_WARN_MAX_GLASS_VERTS = 48,
    R_WARN_MAX_MARK_INDS = 49,
    R_WARN_MAX_MARK_VERTS = 50,
    R_WARN_MAX_SPARK_VERTS = 51,
    R_WARN_MAX_TRAIL_ELEMS_PER_TRAIL = 52,
    R_WARN_TRAIL_WITH_DELAY = 53,
    R_WARN_DEBUG_ALLOC = 54,
    R_WARN_SPOT_LIGHT_LIMIT = 55,
    R_WARN_FX_EFFECT_LIMIT = 56,
    R_WARN_FX_ELEM_LIMIT = 57,
    R_WARN_FX_BOLT_LIMIT = 58,
    R_WARN_WORKER_CMD_SIZE = 59,
    R_WARN_PHYSICS_BODY = 60,
    R_WARN_PHYSICS_JOINT = 61,
    R_WARN_UNKNOWN_STATICMODEL_SHADER = 62,
    R_WARN_UNKNOWN_XMODEL_SHADER = 63,
    R_WARN_DYNAMIC_INDEX_BUFFER_SIZE = 64,
    R_WARN_TOO_MANY_LIGHT_GRID_POINTS = 65,
    R_WARN_FOGABLE_2DTEXT = 66,
    R_WARN_FOGABLE_2DGLYPH = 67,
    R_WARN_OCCLUSION_QUERY = 68,
    R_WARN_MAX_OCCLUSION_QUERIES = 69,
    R_WARN_ESTIMATED_BOUNDS_TOO_SMALL_BEGIN = 70,
    R_WARN_ESTIMATED_BOUNDS_TOO_SMALL_BEGIN_STEPBACK = 69,
    R_WARN_ESTIMATED_BOUNDS_TOO_SMALL0 = 70,
    R_WARN_ESTIMATED_BOUNDS_TOO_SMALL1 = 71,
    R_WARN_ESTIMATED_BOUNDS_TOO_SMALL2 = 72,
    R_WARN_ESTIMATED_BOUNDS_TOO_SMALL_END = 73,
    R_WARN_ESTIMATED_BOUNDS_TOO_SMALL_END_STEPBACK = 72,
    R_WARN_GPU_TIMERS_INACCURATE = 73,
    R_WARN_MAX_JOINT_COUNT = 74,
    R_WARN_MAX_CONTACT_LIST = 75,
    R_WARN_MAX_FX_PER_FRAME = 76,
    R_WARN_ROTATED_CAPSULE_TRACE = 77,
    R_WARN_MINIMAP_ASSETS_NOT_PRECACHED = 78,
    R_WARN_DYNAMIC_INDEX_BUFFER_OVERFLOW = 79,
    R_WARN_PRETESS_INDEX_BUFFER_OVERFLOW = 80,
    R_WARN_RING_BUFFER = 81,
    R_WARN_FX_MAX_RETRIGGER = 82,
    R_WARN_FX_MAX_LIGHTGRID_SAMPLE_COUNT = 83,
    R_WARN_NO_SUN_OVERRIDE = 84,
    R_WARN_THERMAL_LIGHT_OVERFLOW = 85,
    R_WARN_FLARE_OVERFLOW = 86,
    R_WARN_MAX_FLARE_VERTS = 87,
    R_WARN_MAX_FLARE_INDICES = 88,
    R_WARN_SMALL_SCENE_SURFS_SIZE = 89,
    R_WARN_DRAWSURF_PREPASS_NOT_NONE = 90,
    R_WARN_DFOG_DISABLED_IN_FAST_FILE = 91,
    R_WARN_NORMAL_FOG_DISABLED_IN_FAST_FILE = 92,
    R_WARN_MATERIAL_OVERRIDE_LIMIT = 93,
    R_WARN_FX_TRACE_LIMIT_EXCEEDED = 94,
    R_WARN_TONEMAP_WORKER_NOT_FAST_ENOUGH = 95,
    R_WARN_COUNT = 96,
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
//SIZE: 0x10C8
struct GfxWorld {
	const char* name;
	const char* baseName;

	//GfxSky* skies; //0x28

    GfxWorldDpvsPlanes dpvsPlanes; //0x58
    /*
    
    */
    unsigned int* cellCasterBits; //0xC50

	//GfxHeroOnlyLight* heroOnlyLights; //0x41F
};

enum HksBytecodeSharingMode
{
	HKS_BYTECODE_SHARING_OFF = 0x0,
	HKS_BYTECODE_SHARING_ON = 0x1,
	HKS_BYTECODE_SHARING_SECURE = 0x2,
};

enum HksCompilerSettings_IntLiteralOptions
{
    INT_LITERALS_NONE = 0x0,
    INT_LITERALS_LUD = 0x1,
    INT_LITERALS_32BIT = 0x1,
    INT_LITERALS_UI64 = 0x2,
    INT_LITERALS_64BIT = 0x2,
    INT_LITERALS_ALL = 0x3,
};

struct HksCompilerSettings
{
    int m_emitStructCode;
    int i1;
    int i2;
    int i3;
    int m_emitGlobalMemoization;
    int _m_isHksGlobalMemoTestingMode;
    HksBytecodeSharingMode m_bytecodeSharingMode;
    HksCompilerSettings_IntLiteralOptions m_enableIntLiterals;
    int(__fastcall* m_debugMap)(const char*, int);
};

/*

        struct HksCompilerSettings
        {
            int m_emitStructCode;
            const char** m_stripNames;
            int m_emitGlobalMemoization;
            int _m_isHksGlobalMemoTestingMode;
            HksCompilerSettings_BytecodeSharingFormat m_bytecodeSharingFormat;
            HksCompilerSettings_IntLiteralOptions m_enableIntLiterals;
            int(__fastcall* m_debugMap)(const char*, int);
        };
        */

struct HksGlobal {
    __int64 idk0;
    __int64 idk1;
    __int64 idk2;
    HksBytecodeSharingMode m_bytecodeSharingMode;
    char pad[0x53C];
    HksCompilerSettings m_compilerSettings;
};

struct lua_State {
	HksGlobal* m_global;

};

struct GfxBuildInfo {
	const char* bspCommandline;
	const char* lightCommandline;
	const char* bspTimestamp;
	const char* lightTimestamp;
};
struct CardMemory {
	unsigned int platform[1];
};
//WIP
struct GfxImage {
	const char* name; //0x0
    void* ptr1; //0x8 These two pointers are missing about half the time
    void* ptr2; //0x10
    char unknown[0x8]; //0x18
    void* ptr3; //0x20 very rarely a ptr here
    unsigned short width; //0x28
    unsigned short height; //0x2B
    unsigned short depth;
    unsigned short unk;

    char idk[0x18];
    int imageFormat;
    unsigned short w;
    unsigned short h;
    unsigned short d;
    unsigned short height2;//0x52 these are not always used
    unsigned int cardmemory;
    //unsigned short mipcount2; //0x56

    char idk2;
    unsigned char semantic;
    unsigned char category;
    unsigned char flags;
    unsigned char levelCount;
    char idk3[3];
};


union OmnvarValue
{
	bool enabled;
	int integer;
	unsigned int time;
	float value;
	unsigned int ncsString;
};

struct OmnvarData
{
	unsigned int timeModified;
	OmnvarValue current;
};

struct AttHybridSettings {
	float adsSpread;
	float adsAimPitch;
	float adsTransInTime;
	float adsTransInFromSprintTime;
	float adsTransOutTime;
	int adsReloadTransTime;
	float adsCrosshairInFrac;
	float adsCrosshairOutFrac;
	float adsZoomFov;
	float adsZoomInFrac;
	float adsZoomOutFrac;
	float adsFovLerpInTime;
	float adsFovLerpOutTime;
	float adsBobFactor;
	float adsViewBobMult;
	float adsViewErrorMin;
	float adsViewErrorMax;
	float adsFireAnimFrac;
};


//this is wrong lol; correct size tho
struct WeaponAttachment {
	const char* szInternalName;
	const char* szDisplayName;
	int type;
	int weaponType;
	int weapClass;
	int greebleType;
	void** worldModels;
	void** viewModels;
	void** reticleViewModels;
	void* chargeInfo;
	AttHybridSettings* hybridSettings;
	unsigned short* fieldOffsets;
	void* fields;
	int numFields;
	int loadIndex;
	int adsSettingsMode;
	float adsSceneBlurStrength;
	int knifeAttachTagOverride;
	BOOL hideIronSightsWithThisAttachment;
	BOOL showMasterRail;
	BOOL showSideRail;
	BOOL shareAmmoWithAlt;
	BOOL knifeAlwaysAttached;
	BOOL useDualFOV;
	BOOL riotShield;
	BOOL adsSceneBlur;
	BOOL automaticAttachment;
};
union XAssetHeader {
    ScriptFile* script;
    RawFile* rawfile;
    StringTable* table;
    LocalizeEntry* localize;
    GfxImage* image;
    void* data;
};