// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>

#include "player.hh"

namespace rules {

struct Options
{
    // Path to the champion library
    std::string champion_lib;

    // Map file name
    std::string map_file;

    // Memory limit for the champion in MB
    uint32_t memory;

    // Turn time limit for the champion in ms
    uint32_t time;

    // Player
    Player_sptr player;

    // Players
    Players_sptr players;

    // Spactators
    Players_sptr spectators;

    // Verbosity of the logs displayed/written
    unsigned verbose;

    // Game data dump output stream
    std::shared_ptr<std::ostream> dump_stream;
};

} // namespace rules
