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

enum XAssetType {
	ASSET_TYPE_PHYSPRESET = 0x0,
    ASSET_TYPE_XMODEL = 0xA,
    ASSET_TYPE_MATERIAL = 0xD,
    ASSET_TYPE_GFXWORLD = 0x27,
    ASSET_TYPE_LIGHT_DEF = 0x2B,
	ASSET_TYPE_LOCALIZE_ENTRY = 0x2E,
    ASSET_TYPE_WEAPON = 0x30,
    ASSET_TYPE_RAWFILE = 0x39,
    ASSET_TYPE_SCRIPTFILE = 0x3A,
    ASSET_TYPE_STRINGTABLE = 0x3B,
	ASSET_TYPE_TRACER = 0x40,
	ASSET_TYPE_XMODELALIAS = 0x41,
	ASSET_TYPE_ADDON_MAP_ENTS = 0x42,
    ASSET_TYPE_LUA_FILE = 0x45,
	ASSET_TYPE_LASER = 0x4A,
    ASSET_TYPE_FONT = 0x4E,
	ASSET_TYPE_DLOGSCHEMA = 0x51,
};

union XAssetHeader {
    struct ScriptFile* script;
    struct RawFile* rawfile;
    struct StringTable* table;
	void* data;
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

struct lua_State;
struct HashTable;
struct cclosure;

struct GenericChunkHeader
{
	unsigned __int64 m_flags;
};

struct ChunkHeader : GenericChunkHeader
{
	ChunkHeader* m_next;
};

struct UserData : ChunkHeader
{
	unsigned __int64 m_envAndSizeOffsetHighBits;
	unsigned __int64 m_metaAndSizeOffsetLowBits;
	char m_data[8];
};

struct InternString
{
	unsigned __int64 m_flags;
	unsigned __int64 m_lengthbits;
	unsigned int m_hash;
	char m_data[30];
};

union HksValue
{
	cclosure* cClosure;
	void* closure;
	UserData* userData;
	HashTable* table;
	void* tstruct;
	InternString* str;
	void* thread;
	void* ptr;
	float number;
	long long i64;
	unsigned long long ui64;
	unsigned int native;
	bool boolean;
};

enum HksObjectType
{
	TANY = 0xFFFFFFFE,
	TNONE = 0xFFFFFFFF,
	TNIL = 0x0,
	TBOOLEAN = 0x1,
	TLIGHTUSERDATA = 0x2,
	TNUMBER = 0x3,
	TSTRING = 0x4,
	TTABLE = 0x5,
	TFUNCTION = 0x6,  // idk
	TUSERDATA = 0x7,
	TTHREAD = 0x8,
	TIFUNCTION = 0x9, // Lua function
	TCFUNCTION = 0xA, // C function
	TUI64 = 0xB,
	TSTRUCT = 0xC,
	NUM_TYPE_OBJECTS = 0xE,
};

struct HksObject
{
	HksObjectType t;
	HksValue v;
};

const struct hksInstruction
{
	unsigned int code;
};

struct ActivationRecord
{
	HksObject* m_base;
	const hksInstruction* m_returnAddress;
	__int16 m_tailCallDepth;
	__int16 m_numVarargs;
	int m_numExpectedReturns;
};

struct CallStack
{
	ActivationRecord* m_records;
	ActivationRecord* m_lastrecord;
	ActivationRecord* m_current;
	const hksInstruction* m_current_lua_pc;
	const hksInstruction* m_hook_return_addr;
	int m_hook_level;
};

struct ApiStack
{
	HksObject* top;
	HksObject* base;
	HksObject* alloc_top;
	HksObject* bottom;
};

struct UpValue : ChunkHeader
{
	HksObject m_storage;
	HksObject* loc;
	UpValue* m_next;
};

struct CallSite
{
	_SETJMP_FLOAT128 m_jumpBuffer[16];
	CallSite* m_prev;
};

enum Status
{
	NEW = 0x1,
	RUNNING = 0x2,
	YIELDED = 0x3,
	DEAD_ERROR = 0x4,
};

enum HksError
{
	HKS_NO_ERROR = 0x0,
	HKS_ERRSYNTAX = 0xFFFFFFFC,
	HKS_ERRFILE = 0xFFFFFFFB,
	HKS_ERRRUN = 0xFFFFFF9C,
	HKS_ERRMEM = 0xFFFFFF38,
	HKS_ERRERR = 0xFFFFFED4,
	HKS_THROWING_ERROR = 0xFFFFFE0C,
	HKS_GC_YIELD = 0x1,
};

struct lua_Debug
{
	int event;
	const char* name;
	const char* namewhat;
	const char* what;
	const char* source;
	int currentline;
	int nups;
	int nparams;
	int ishksfunc;
	int linedefined;
	int lastlinedefined;
	char short_src[512];
	int callstack_level;
	int is_tail_call;
};

using lua_function = int(__fastcall*)(lua_State*);

struct luaL_Reg
{
	const char* name;
	lua_function function;
};

struct Node
{
	HksObject m_key;
	HksObject m_value;
};

struct Metatable
{
};

struct HashTable : ChunkHeader
{
	Metatable* m_meta;
	unsigned int m_version;
	unsigned int m_mask;
	Node* m_hashPart;
	HksObject* m_arrayPart;
	unsigned int m_arraySize;
	Node* m_freeNode;
};

struct cclosure : ChunkHeader
{
	lua_function m_function;
	HashTable* m_env;
	__int16 m_numUpvalues;
	__int16 m_flags;
	InternString* m_name;
	HksObject m_upvalues[1];
};

enum HksCompilerSettings_BytecodeSharingFormat
{
	BYTECODE_DEFAULT = 0x0,
	BYTECODE_INPLACE = 0x1,
	BYTECODE_REFERENCED = 0x2,
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
	const char** m_stripNames;
	int m_emitGlobalMemoization;
	int _m_isHksGlobalMemoTestingMode;
	HksCompilerSettings_BytecodeSharingFormat m_bytecodeSharingFormat;
	HksCompilerSettings_IntLiteralOptions m_enableIntLiterals;
	int(__fastcall* m_debugMap)(const char*, int);
};

enum HksBytecodeSharingMode
{
	HKS_BYTECODE_SHARING_OFF = 0x0,
	HKS_BYTECODE_SHARING_ON = 0x1,
	HKS_BYTECODE_SHARING_SECURE = 0x2,
};

struct HksGcWeights
{
	int m_removeString;
	int m_finalizeUserdataNoMM;
	int m_finalizeUserdataGcMM;
	int m_cleanCoroutine;
	int m_removeWeak;
	int m_markObject;
	int m_traverseString;
	int m_traverseUserdata;
	int m_traverseCoroutine;
	int m_traverseWeakTable;
	int m_freeChunk;
	int m_sweepTraverse;
};

struct GarbageCollector_Stack
{
	void* m_storage;
	unsigned int m_numEntries;
	unsigned int m_numAllocated;
};

struct ProtoList
{
	void** m_protoList;
	unsigned __int16 m_protoSize;
	unsigned __int16 m_protoAllocSize;
};

struct GarbageCollector
{
	int m_target;
	int m_stepsLeft;
	int m_stepLimit;
	HksGcWeights m_costs;
	int m_unit;
	_SETJMP_FLOAT128(*m_jumpPoint)[16];
	lua_State* m_mainState;
	lua_State* m_finalizerState;
	void* m_memory;
	int m_phase;
	GarbageCollector_Stack m_resumeStack;
	GarbageCollector_Stack m_greyStack;
	GarbageCollector_Stack m_remarkStack;
	GarbageCollector_Stack m_weakStack;
	int m_finalizing;
	HksObject m_safeTableValue;
	lua_State* m_startOfStateStackList;
	lua_State* m_endOfStateStackList;
	lua_State* m_currentState;
	HksObject m_safeValue;
	void* m_compiler;
	void* m_bytecodeReader;
	void* m_bytecodeWriter;
	int m_pauseMultiplier;
	int m_stepMultiplier;
	bool m_stopped;
	int(__fastcall* m_gcPolicy)(lua_State*);
	unsigned __int64 m_pauseTriggerMemoryUsage;
	int m_stepTriggerCountdown;
	unsigned int m_stringTableIndex;
	unsigned int m_stringTableSize;
	UserData* m_lastBlackUD;
	UserData* m_activeUD;
};

enum MemoryManager_ChunkColor
{
	RED = 0x0,
	BLACK = 0x1,
};

struct ChunkList
{
	ChunkHeader m_prevToStart;
};

enum Hks_DeleteCheckingMode
{
	HKS_DELETE_CHECKING_OFF = 0x0,
	HKS_DELETE_CHECKING_ACCURATE = 0x1,
	HKS_DELETE_CHECKING_SAFE = 0x2,
};

struct MemoryManager
{
	void* (__fastcall* m_allocator)(void*, void*, unsigned __int64, unsigned __int64);
	void* m_allocatorUd;
	MemoryManager_ChunkColor m_chunkColor;
	unsigned __int64 m_used;
	unsigned __int64 m_highwatermark;
	ChunkList m_allocationList;
	ChunkList m_sweepList;
	ChunkHeader* m_lastKeptChunk;
	lua_State* m_state;
	ChunkList m_deletedList;
	int m_deleteMode;
	Hks_DeleteCheckingMode m_deleteCheckingMode;
};

struct StaticStringCache
{
	HksObject m_objects[41];
};

enum HksBytecodeEndianness
{
	HKS_BYTECODE_DEFAULT_ENDIAN = 0x0,
	HKS_BYTECODE_BIG_ENDIAN = 0x1,
	HKS_BYTECODE_LITTLE_ENDIAN = 0x2,
};

struct RuntimeProfileData_Stats
{
	unsigned __int64 hksTime;
	unsigned __int64 callbackTime;
	unsigned __int64 gcTime;
	unsigned __int64 cFinalizerTime;
	unsigned __int64 compilerTime;
	unsigned int hkssTimeSamples;
	unsigned int callbackTimeSamples;
	unsigned int gcTimeSamples;
	unsigned int compilerTimeSamples;
	unsigned int num_newuserdata;
	unsigned int num_tablerehash;
	unsigned int num_pushstring;
	unsigned int num_pushcfunction;
	unsigned int num_newtables;
};

struct RuntimeProfileData
{
	__int64 stackDepth;
	__int64 callbackDepth;
	unsigned __int64 lastTimer;
	RuntimeProfileData_Stats frameStats;
	unsigned __int64 gcStartTime;
	unsigned __int64 finalizerStartTime;
	unsigned __int64 compilerStartTime;
	unsigned __int64 compilerStartGCTime;
	unsigned __int64 compilerStartGCFinalizerTime;
	unsigned __int64 compilerCallbackStartTime;
	__int64 compilerDepth;
	void* outFile;
	lua_State* rootState;
};

//taken straight from s1-mod so need to double check everything for lui


struct HksGlobal
{
	MemoryManager m_memory;
	GarbageCollector m_collector;
	StringTable m_stringTable;
	HksBytecodeSharingMode m_bytecodeSharingMode;
	unsigned int m_tableVersionInitializer;
	HksObject m_registry;
	ProtoList m_protoList;
	HashTable* m_structProtoByName;
	ChunkList m_userDataList;
	lua_State* m_root;
	StaticStringCache m_staticStringCache;
	void* m_debugger;
	void* m_profiler;
	RuntimeProfileData m_runProfilerData;
	HksCompilerSettings m_compilerSettings;
	int(__fastcall* m_panicFunction)(lua_State*);
	void* m_luaplusObjectList;
	int m_heapAssertionFrequency;
	int m_heapAssertionCount;
	void (*m_logFunction)(lua_State*, const char*, ...);
	HksBytecodeEndianness m_bytecodeDumpEndianness;
};

struct lua_State : ChunkHeader
{
	HksGlobal* m_global;
	CallStack m_callStack;
	ApiStack m_apistack;
	UpValue* pending;
	HksObject globals;
	HksObject m_cEnv;
	CallSite* m_callsites;
	int m_numberOfCCalls;
	void* m_context;
	InternString* m_name;
	lua_State* m_nextState;
	lua_State* m_nextStateStack;
	Status m_status;
	HksError m_error;
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
	/*
	16 bytes of DX buffer?
	*/
	char unknown[0x40]; //0x8
	int format; //0x48
	unsigned short width; //0x4C
	unsigned short height; //0x4E
	unsigned short depth; //0x50
	unsigned short mipCount; //0x52
	CardMemory cardMemory; //0x54
	unsigned char mapType; //0x58
	unsigned char _pad0[3]; //just to ensure alignment
	unsigned char levelCount; //0x5C
};
static_assert(offsetof(GfxImage, name) == 0x0);
static_assert(offsetof(GfxImage, format) == 0x48);
static_assert(offsetof(GfxImage, width) == 0x4C);
static_assert(offsetof(GfxImage, height) == 0x4E);
static_assert(offsetof(GfxImage, depth) == 0x50);
static_assert(offsetof(GfxImage, mipCount) == 0x52);
static_assert(offsetof(GfxImage, cardMemory) == 0x54);
static_assert(offsetof(GfxImage, mapType) == 0x58);
static_assert(offsetof(GfxImage, levelCount) == 0x5C);

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