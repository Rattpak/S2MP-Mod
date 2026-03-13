#pragma once

class Noclip {
public:
	static bool getNoclipState();
	static void init();
	static void toggle();
private:
	static bool isActive;
};