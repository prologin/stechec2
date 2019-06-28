// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
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

    // Map file name. Deprecated: use map_content.
    std::string map_file;

    // Map content
    std::string map_content;

    // Memory limit for the champion in MB
    uint32_t memory;

    // Turn time limit for the champion in ms
    uint32_t time;

    // Player
    Player_sptr player;

    // Players
    Players_sptr players;

    // Spectators
    Players_sptr spectators;

    // Verbosity of the logs displayed/written
    unsigned verbose;

    // Game data dump output stream
    std::shared_ptr<std::ostream> dump_stream;

    // Replay data dump output stream
    std::shared_ptr<std::ostream> replay_stream;
};

std::string read_map_from_path(const std::string& path);

} // namespace rules
