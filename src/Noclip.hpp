#pragma once
#include <string>
class Noclip {
public:
	static uintptr_t base;
	static void init();
	static void toggle();
private:
	static bool isActive;
};