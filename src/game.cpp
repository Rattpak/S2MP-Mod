#include "pch.h"
#include "game.h"

uintptr_t base = (uintptr_t)GetModuleHandle(NULL) + 0x1000;

size_t _b(const size_t val) { return base + val; }
size_t operator"" _b(const size_t val) { return _b(val); }