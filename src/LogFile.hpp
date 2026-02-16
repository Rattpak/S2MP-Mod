#pragma once
#include <string>
#include "Structs.h"

class Logfile {
public:
	static void setEnabled(bool b);
	static void append(std::string& text);
	static void init();
private:
	static bool enabled;
};