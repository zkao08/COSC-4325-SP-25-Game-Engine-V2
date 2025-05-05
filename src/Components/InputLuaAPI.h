#pragma once

#include "InputHandler.h"
#include <sol/sol.hpp>

namespace InputLuaAPI
{
    // Initialize the Lua API
    void Initialize(sol::state& lua);
}