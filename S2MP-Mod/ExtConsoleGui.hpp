#pragma once
#include <windows.h>
class ExternalConsoleGui {
public:
	static int init(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow);
	static void print(const std::string& s);
	static bool isExtConGuiReady();
};