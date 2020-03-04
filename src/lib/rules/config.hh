// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <cstdint>

namespace rules
{

// Game-specific configuration that the server enforces
struct Config
{
    // The name of the game
    char const* name;
    // The number of players in the game
    int32_t player_count;
};

} // namespace rules
