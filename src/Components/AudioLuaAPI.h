#pragma once

#include "AudioManager.h"
#include "SoundResource.h"
#include "ResourceManager.h"
#include <sol/sol.hpp>

namespace AudioLuaAPI
{
    // Initialize the Lua API
    void Initialize(sol::state& lua);
}