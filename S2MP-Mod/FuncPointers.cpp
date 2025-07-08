/////////////////////////////////////
//       Func Pointers
//	Engine Function Pointers
/////////////////////////////////////
#include "pch.h"
#include "FuncPointers.h"

uintptr_t base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;

Functions::SV_SendServerCommand Functions::_SV_SendServerCommand = (Functions::SV_SendServerCommand)(base + 0x7485E0);
Functions::Com_Error Functions::_Com_Error = (Functions::Com_Error)(base + 0xAA1E0);
Functions::Scr_MakeGameMessage Functions::_Scr_MakeGameMessage = (Functions::Scr_MakeGameMessage)(base + 0x613230);
Functions::Sys_EnterCriticalSection Functions::_Sys_EnterCriticalSection = (Functions::Sys_EnterCriticalSection)(base + 0x7D6B50);
Functions::Sys_LeaveCriticalSection Functions::_Sys_LeaveCriticalSection = (Functions::Sys_LeaveCriticalSection)(base + 0x7D6BC0);
Functions::Sys_Cwd Functions::_Sys_Cwd = (Functions::Sys_Cwd)(base + 0x7D6980);
Functions::Dvar_FindVar Functions::_Dvar_FindVar = (Functions::Dvar_FindVar)(base + 0x108C30);
Functions::SEH_SafeTranslateString Functions::_SEH_SafeTranslateString = (Functions::SEH_SafeTranslateString)(base + 0x7934D0);
Functions::SEH_StringEd_GetString Functions::_SEH_StringEd_GetString = (Functions::SEH_StringEd_GetString)(base + 0x793520);
Functions::Com_Quit_f Functions::_Com_Quit_f = (Functions::Com_Quit_f)(base + 0xF4430);
Functions::GetAvailableCommandBufferIndex Functions::_GetAvailableCommandBufferIndex = (Functions::GetAvailableCommandBufferIndex)(base + 0x503030);
Functions::SV_MapRestart_f Functions::_SV_MapRestart_f = (Functions::SV_MapRestart_f)(base + 0x73E800);

Functions::R_AddCmdDrawText Functions::_R_AddCmdDrawText = (Functions::R_AddCmdDrawText)(base + 0x92CDE0);
Functions::R_RegisterFont Functions::_R_RegisterFont = (Functions::R_RegisterFont)(base + 0x906F50);
Functions::UI_RunMenuScript Functions::_UI_RunMenuScript = (Functions::UI_RunMenuScript)(base + 0x7ADA80);
Functions::SV_Loaded Functions::_SV_Loaded = (Functions::SV_Loaded)(base + 0x7438D0);
Functions::R_AddCmdDrawStretchPic Functions::_R_AddCmdDrawStretchPic = (Functions::R_AddCmdDrawStretchPic)(base + 0x92C460);
Functions::R_AddCmdDrawTextWithCursor Functions::_R_AddCmdDrawTextWithCursor = (Functions::R_AddCmdDrawTextWithCursor)(base + 0x92D670);
Functions::LUI_OpenMenu Functions::_LUI_OpenMenu = (Functions::LUI_OpenMenu)(base + 0x7AC7A0);
Functions::LiveStorage_UploadStats Functions::_LiveStorage_UploadStats = (Functions::LiveStorage_UploadStats)(base + 0x6C38A0);
Functions::Cmd_AddCommandInternal Functions::_Cmd_AddCommandInternal = (Functions::Cmd_AddCommandInternal)(base + 0x6AE0E0);