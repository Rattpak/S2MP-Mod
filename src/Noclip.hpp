#pragma once

class Noclip {
public:
	static void init();
	static void toggle();
private:
	static bool isActive;
};