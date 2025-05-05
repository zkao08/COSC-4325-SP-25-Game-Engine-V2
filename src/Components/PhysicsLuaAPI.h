// PhysicsLuaAPI.h
#pragma once

#include "PhysicsWorld.h"
#include <sol/sol.hpp>

namespace PhysicsLuaAPI
{
    // Initialize the Physics Lua API
    void Initialize(sol::state& lua);
}