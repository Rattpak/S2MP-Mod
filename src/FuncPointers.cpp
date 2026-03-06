/////////////////////////////////////
//       Func Pointers
//	Engine Function Pointers
/////////////////////////////////////
#include "pch.h"
#include "FuncPointers.h"
#include "Console.hpp"
#include "DevDef.h"

Functions::SV_StartMap Functions::_SV_StartMap = nullptr;
Functions::GetStringFromResource Functions::_GetStringFromResource = nullptr;
Functions::SV_SendServerCommand Functions::_SV_SendServerCommand = nullptr;
Functions::Com_Error Functions::_Com_Error = nullptr;
Functions::Scr_MakeGameMessage Functions::_Scr_MakeGameMessage = nullptr;
Functions::Sys_EnterCriticalSection Functions::_Sys_EnterCriticalSection = nullptr;
Functions::Sys_LeaveCriticalSection Functions::_Sys_LeaveCriticalSection = nullptr;
Functions::Sys_Cwd Functions::_Sys_Cwd = nullptr;
Functions::Dvar_FindVar Functions::_Dvar_FindVar = nullptr;
Functions::SEH_SafeTranslateString Functions::_SEH_SafeTranslateString = nullptr;
Functions::SEH_StringEd_GetString Functions::_SEH_StringEd_GetString = nullptr;
Functions::Com_Quit_f Functions::_Com_Quit_f = nullptr;
Functions::GetAvailableCommandBufferIndex Functions::_GetAvailableCommandBufferIndex = nullptr;
Functions::SV_MapRestart_f Functions::_SV_MapRestart_f = nullptr;

Functions::R_AddCmdDrawText Functions::_R_AddCmdDrawText = nullptr;
Functions::R_RegisterFont Functions::_R_RegisterFont = nullptr;
Functions::UI_RunMenuScript Functions::_UI_RunMenuScript = nullptr;
Functions::SV_Loaded Functions::_SV_Loaded = nullptr;
Functions::R_AddCmdDrawStretchPic Functions::_R_AddCmdDrawStretchPic = nullptr;
Functions::R_AddCmdDrawTextWithCursor Functions::_R_AddCmdDrawTextWithCursor = nullptr;
Functions::LUI_OpenMenu Functions::_LUI_OpenMenu = nullptr;
Functions::LiveStorage_UploadStats Functions::_LiveStorage_UploadStats = nullptr;
Functions::Cmd_AddCommandInternal Functions::_Cmd_AddCommandInternal = nullptr;

Functions::LUI_CoD_GetMaxMemory Functions::_LUI_CoD_GetMaxMemory = nullptr;
Functions::LUI_CoD_GetFreeMemoryBytes Functions::_LUI_CoD_GetFreeMemoryBytes = nullptr;
Functions::hks_HashTable_contiguousArraySize Functions::_hks_HashTable_contiguousArraySize = nullptr;
Functions::Material_RegisterHandle Functions::_Material_RegisterHandle = nullptr;
Functions::Dvar_RegisterBool Functions::_Dvar_RegisterBool = nullptr;

void Functions::init()
{
	DEV_INIT_PRINT();
	_Sys_Cwd = (Sys_Cwd)(0x771750_b);
	_SV_StartMap = (SV_StartMap)(0x6D7200_b);
	_GetStringFromResource = (GetStringFromResource)(0x7894A0_b);
	_SV_SendServerCommand = (SV_SendServerCommand)(0x6DFBA0_b);
	_Com_Error = (Com_Error)(0x8F750_b);
	_Scr_MakeGameMessage = (Scr_MakeGameMessage)(0x5AE7A0_b);
	_Sys_EnterCriticalSection = (Sys_EnterCriticalSection)(0x7719B0_b);
	_Sys_LeaveCriticalSection = (Sys_LeaveCriticalSection)(0x771A20_b);
	_Dvar_FindVar = (Dvar_FindVar)(0xAE9D0_b);
	_SEH_SafeTranslateString = (SEH_SafeTranslateString)(0x72CE50_b);
	_SEH_StringEd_GetString = (SEH_StringEd_GetString)(0x72CEA0_b);
	_Dvar_RegisterBool = (Dvar_RegisterBool)(0xAF5E0_b);
	_Com_Quit_f = (Com_Quit_f)(0x99130_b);
	_GetAvailableCommandBufferIndex = (GetAvailableCommandBufferIndex)(0x4A05C0_b);
	_SV_MapRestart_f = (SV_MapRestart_f)(0x6D5B50_b);
	_R_AddCmdDrawText = (R_AddCmdDrawText)(0x8B9480_b);
	_R_RegisterFont = (R_RegisterFont)(0x892D90_b);
	_UI_RunMenuScript = (UI_RunMenuScript)(0x745A50_b);
	_SV_Loaded = (SV_Loaded)(0x6DF9D6_b);
	_R_AddCmdDrawStretchPic = (R_AddCmdDrawStretchPic)(0x8B8A00_b);
	_R_AddCmdDrawTextWithCursor = (R_AddCmdDrawTextWithCursor)(0x8B9D10_b);
	_LUI_OpenMenu = (LUI_OpenMenu)(0x743F60_b);
	_LiveStorage_UploadStats = (LiveStorage_UploadStats)(0x65BA90_b);
	_Cmd_AddCommandInternal = (Cmd_AddCommandInternal)(0x6496B0_b);
	_LUI_CoD_GetMaxMemory = (LUI_CoD_GetMaxMemory)(0x180CA0_b);
	_LUI_CoD_GetFreeMemoryBytes = (LUI_CoD_GetFreeMemoryBytes)(0x180C90_b);
	_hks_HashTable_contiguousArraySize = (hks_HashTable_contiguousArraySize)(0x2CA7D0_b);
	_Material_RegisterHandle = (Material_RegisterHandle)(0x8AB5F0_b);
}
