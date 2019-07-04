// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2015 Association Prologin <association@prologin.org>

#include "game_state.hh"

#include <sstream>

GameState::GameState(const std::string& map_content,
                     const rules::Players& players)
    : rules::GameState(players), secret_number_found(false), round(0)
{
    std::istringstream map_stream{map_content};
    map_stream >> secret_number;
    for (const auto& p : players_.all())
        player_guess_map[p->id] = -2; // default value
}

GameState* GameState::copy() const
{
    return new GameState(*this);
}

bool GameState::is_finished() const
{
    return secret_number_found || round >= 100;
}
