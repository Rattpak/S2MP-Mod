#pragma once

#include "Console.hpp"

namespace game
{
    class dvar
    {
    public:
        dvar() {}
        ~dvar() {}

        void Register(const char* name, int default_value, int min, int max, unsigned int flags)
        {
            _raw = game::Dvar_RegisterInt(name, default_value, min, max, flags);
            Console::Print(Console::dev, "registering dvar '%s' with value '%i'", name, default_value);
        }

        void Register(const char* name, bool default_value, unsigned int flags)
        {
            _raw = game::Dvar_RegisterBool(name, default_value, flags);
            Console::Print(Console::dev, "registering dvar '%s' with value '%s'", name, default_value ? "true" : "false");
        }

        void Register(const char* name, const char* default_value, unsigned int flags)
        {
            _raw = game::Dvar_RegisterString(name, default_value, flags);
            Console::Print(Console::dev, "registering dvar '%s' with value '%s'", name, default_value);
        }

        dvar_t* get_raw()
        {
            return this->_raw;
        }

    private:
        dvar_t* _raw = nullptr;
    };
}
