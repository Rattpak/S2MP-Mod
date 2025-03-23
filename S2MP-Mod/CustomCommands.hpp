#pragma once
#include <string>
class CustomCommands {
public:
	static uintptr_t base;
	static uintptr_t rawBase;
	static void toggleGodmode();
	static void toggleHud(bool b);
	static void toggleHudBlood(bool b);
	static void toggleGun(bool b);
	static void toggleFog(bool b);
	static void changeMap(const std::string& mapname);
	static void fastRestart();
	static void mapRestart();
private:
	static bool isGodmode;
};