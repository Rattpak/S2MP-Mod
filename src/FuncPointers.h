#pragma once
#include "structs.h"

class Functions {
public:
	static void init();

	typedef char* (__cdecl* GetStringFromResource)(UINT num);
	static GetStringFromResource _GetStringFromResource;

	typedef void(__cdecl* SV_SendServerCommand)(__int64 client, int type, const char* fmt, ...);
	static SV_SendServerCommand _SV_SendServerCommand;

	typedef void(__cdecl* Com_Error)(errorParm_t code, const char* fmt, ...);
	static Com_Error _Com_Error;

	typedef void(__cdecl* Scr_MakeGameMessage)(const char *msg);
	static Scr_MakeGameMessage _Scr_MakeGameMessage;

	typedef void(__cdecl* Sys_EnterCriticalSection)(int critSect);
	static Sys_EnterCriticalSection _Sys_EnterCriticalSection;

	typedef void(__cdecl* Sys_LeaveCriticalSection)(int critSect);
	static Sys_LeaveCriticalSection _Sys_LeaveCriticalSection;

	typedef const char* (__cdecl* Sys_Cwd)();
	static Sys_Cwd _Sys_Cwd;

	typedef dvar_t* (__cdecl* Dvar_FindVar)(const char* dvarName);
	static Dvar_FindVar _Dvar_FindVar;

	typedef char* (__cdecl* SEH_SafeTranslateString)(const char* pszReference);
	static SEH_SafeTranslateString _SEH_SafeTranslateString;

	typedef char* (__cdecl* SEH_StringEd_GetString)(const char* pszReference);
	static SEH_StringEd_GetString _SEH_StringEd_GetString;

	typedef void (__cdecl* Com_Quit_f)();
	static Com_Quit_f _Com_Quit_f;

	typedef __int64 (__cdecl* GetAvailableCommandBufferIndex)();
	static GetAvailableCommandBufferIndex _GetAvailableCommandBufferIndex;

	typedef void (__cdecl* SV_MapRestart_f)();
	static SV_MapRestart_f _SV_MapRestart_f;

	typedef void(__cdecl* R_AddCmdDrawText)(const char* text, int max_chars, font_t* font, int unk0, int unk1, int pixel_h, float pos_x, float pos_y, float scale_x, float scale_y, float rotation, float* color, long long style);
	static R_AddCmdDrawText _R_AddCmdDrawText;

	typedef font_t* (__cdecl* R_RegisterFont)(const char* name, int size);
	static R_RegisterFont _R_RegisterFont;

	typedef void(__cdecl* UI_RunMenuScript)(int client, const char** args);
	static UI_RunMenuScript _UI_RunMenuScript;

	typedef bool(__cdecl* SV_Loaded)();
	static SV_Loaded _SV_Loaded;

	typedef bool(__cdecl* R_AddCmdDrawStretchPic)(float x, float y, float w, float h, float xScale, float yScale, float xay, float yay, float* color, material_t* material);
	static R_AddCmdDrawStretchPic _R_AddCmdDrawStretchPic;

	typedef bool(__cdecl* R_AddCmdDrawTextWithCursor)(const char* text, int max_chars, font_t* font, int unk0, int unk1, int pixel_h, float pos_x, float pos_y, float scale_x, float scale_y, float rotation, const float* color, long long style, int cursorPos, char cursor);
	static R_AddCmdDrawTextWithCursor _R_AddCmdDrawTextWithCursor;

	typedef bool(__cdecl* LUI_OpenMenu)(int client, const char* menu, int a, unsigned int is_exclusive);
	static LUI_OpenMenu _LUI_OpenMenu;

	typedef void(__cdecl* LiveStorage_UploadStats)(int clientNum);
	static LiveStorage_UploadStats _LiveStorage_UploadStats;

	typedef void(__cdecl* Cmd_AddCommandInternal)(char const* name, void (*func)(), cmd_function_s* cmd);
	static Cmd_AddCommandInternal _Cmd_AddCommandInternal;

	typedef int(__cdecl* LUI_CoD_GetMaxMemory)();
	static LUI_CoD_GetMaxMemory _LUI_CoD_GetMaxMemory;

	typedef int(__cdecl* LUI_CoD_GetFreeMemoryBytes)();
	static LUI_CoD_GetFreeMemoryBytes _LUI_CoD_GetFreeMemoryBytes;

	typedef int(__cdecl* hks_HashTable_contiguousArraySize)();
	static hks_HashTable_contiguousArraySize _hks_HashTable_contiguousArraySize;

	typedef material_t* (__cdecl* Material_RegisterHandle)(const char* name);
	static Material_RegisterHandle _Material_RegisterHandle;
};
