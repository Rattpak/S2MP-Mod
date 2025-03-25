#pragma once
#include <string>

class SetScore {
public:
    static uintptr_t base;
    static void init();
    static void set(int playerID, int score);
};
