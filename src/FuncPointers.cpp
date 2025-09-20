/////////////////////////////////////
//       Func Pointers
//	Engine Function Pointers
/////////////////////////////////////
#include "pch.h"
#include "FuncPointers.h"
#include "Console.hpp"

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

void Functions::init()
{
	Console::initPrint("Functions::init()");

	_SV_SendServerCommand = (SV_SendServerCommand)(0x7485E0_b);
	_Com_Error = (Com_Error)(0xAA1E0_b);
	_Scr_MakeGameMessage = (Scr_MakeGameMessage)(0x613230_b);
	_Sys_EnterCriticalSection = (Sys_EnterCriticalSection)(0x7D6B50_b);
	_Sys_LeaveCriticalSection = (Sys_LeaveCriticalSection)(0x7D6BC0_b);
	_Sys_Cwd = (Sys_Cwd)(0x7D6980_b);
	_Dvar_FindVar = (Dvar_FindVar)(0x108C30_b);
	_SEH_SafeTranslateString = (SEH_SafeTranslateString)(0x7934D0_b);
	_SEH_StringEd_GetString = (SEH_StringEd_GetString)(0x793520_b);
	_Com_Quit_f = (Com_Quit_f)(0xF4430_b);
	_GetAvailableCommandBufferIndex = (GetAvailableCommandBufferIndex)(0x503030_b);
	_SV_MapRestart_f = (SV_MapRestart_f)(0x73E800_b);

	_R_AddCmdDrawText = (R_AddCmdDrawText)(0x92CDE0_b);
	_R_RegisterFont = (R_RegisterFont)(0x906F50_b);
	_UI_RunMenuScript = (UI_RunMenuScript)(0x7ADA80_b);
	_SV_Loaded = (SV_Loaded)(0x7438D0_b);
	_R_AddCmdDrawStretchPic = (R_AddCmdDrawStretchPic)(0x92C460_b);
	_R_AddCmdDrawTextWithCursor = (R_AddCmdDrawTextWithCursor)(0x92D670_b);
	_LUI_OpenMenu = (LUI_OpenMenu)(0x7AC7A0_b);
	_LiveStorage_UploadStats = (LiveStorage_UploadStats)(0x6C38A0_b);
	_Cmd_AddCommandInternal = (Cmd_AddCommandInternal)(0x6AE0E0_b);

	_LUI_CoD_GetMaxMemory = (LUI_CoD_GetMaxMemory)(0x1E4CD0_b);
	_LUI_CoD_GetFreeMemoryBytes = (LUI_CoD_GetFreeMemoryBytes)(0x1E4CC0_b);
	_hks_HashTable_contiguousArraySize = (hks_HashTable_contiguousArraySize)(0x32BFA0_b);
	_Material_RegisterHandle = (Material_RegisterHandle)(0x91F430_b);
}
