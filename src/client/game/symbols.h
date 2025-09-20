#pragma once

#include "game.h"
#include "structs.h"

namespace game
{
	static symbol<XAssetHeader(XAssetType type, const char* name, int allowCreateDefault)> DB_FindXAssetHeader{ 0xFAB20_b };
	static symbol<void(const char* name)> ProcessScript{ 0x6F6740_b };
	static symbol<dvar_t* (const char* name)> Dvar_FindMalleableVar{0x108B90_b};
	static symbol<uint64_t (uint64_t, uint64_t, const char* name)> translateHasValue{ 0x167F60_b };
	static symbol<const char* (uint64_t, uint64_t, const char* name)> translateHasValue1{ 0x167F60_b };
	
	static symbol<dvar_t* (const char* name, int default_value, int min, int max, unsigned int flags)> Dvar_RegisterInt{ 0x115040_b };
	static symbol<dvar_t* (const char* name, bool default_value, unsigned int flags)> Dvar_RegisterBool{ 0x1149A0_b };
	static symbol<dvar_t* (const char* name, const char* default_value, unsigned int flags)> Dvar_RegisterString{ 0x1157B0_b };
}	