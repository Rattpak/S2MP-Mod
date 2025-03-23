#pragma once
#include "structs.h"
class Functions {
public:
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
	
	typedef void (__cdecl* Sys_Quit)();
	static Sys_Quit _Sys_Quit;
	
	typedef __int64 (__cdecl* GetAvailableCommandBufferIndex)();
	static GetAvailableCommandBufferIndex _GetAvailableCommandBufferIndex;
	
};
