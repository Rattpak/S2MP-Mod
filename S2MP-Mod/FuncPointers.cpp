/////////////////////////////////////
//       Func Pointers
//	Engine Function Pointers
/////////////////////////////////////
#include "pch.h"
#include "FuncPointers.h"

uintptr_t base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;

Functions::SV_SendServerCommand Functions::_SV_SendServerCommand = (Functions::SV_SendServerCommand)(base + 0x6DC3A0); //updated
Functions::Com_Error Functions::_Com_Error = (Functions::Com_Error)(base + 0x8BA30); //updated
Functions::Scr_MakeGameMessage Functions::_Scr_MakeGameMessage = (Functions::Scr_MakeGameMessage)(base + 0x5AB1B0); //updated
Functions::Sys_EnterCriticalSection Functions::_Sys_EnterCriticalSection = (Functions::Sys_EnterCriticalSection)(base + 0x76EA70);//updated
Functions::Sys_LeaveCriticalSection Functions::_Sys_LeaveCriticalSection = (Functions::Sys_LeaveCriticalSection)(base + 0x76EAE0);//updated
Functions::Sys_Cwd Functions::_Sys_Cwd = (Functions::Sys_Cwd)(base + 0x76E810);//updated
Functions::Dvar_FindVar Functions::_Dvar_FindVar = (Functions::Dvar_FindVar)(base + 0xAAA50);//updated
Functions::SEH_SafeTranslateString Functions::_SEH_SafeTranslateString = (Functions::SEH_SafeTranslateString)(base + 0x7299F0);//updated
Functions::SEH_StringEd_GetString Functions::_SEH_StringEd_GetString = (Functions::SEH_StringEd_GetString)(base + 0x729A40);//updated
Functions::Sys_Quit Functions::_Sys_Quit = (Functions::Sys_Quit)(base + 0x785F70);//updated
Functions::GetAvailableCommandBufferIndex Functions::_GetAvailableCommandBufferIndex = (Functions::GetAvailableCommandBufferIndex)(base + 0x49CE70);//updated
Functions::SV_MapRestart_f Functions::_SV_MapRestart_f = (Functions::SV_MapRestart_f)(base + 0x6D2360);//updated

Functions::R_AddCmdDrawText Functions::_R_AddCmdDrawText = (Functions::R_AddCmdDrawText)(base + 0x8AE970);
Functions::R_RegisterFont Functions::_R_RegisterFont = (Functions::R_RegisterFont)(base + 0x888320);
Functions::UI_RunMenuScript Functions::_UI_RunMenuScript = (Functions::UI_RunMenuScript)(base + 0x742520);
Functions::SV_Loaded Functions::_SV_Loaded = (Functions::SV_Loaded)(base + 0x6D7090);