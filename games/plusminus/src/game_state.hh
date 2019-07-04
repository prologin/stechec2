// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2015 Association Prologin <association@prologin.org>
#pragma once

#include <unordered_map>

#include <rules/game-state.hh>
#include <rules/player.hh>

class GameState final : public rules::GameState
{
public:
    GameState(const std::string& map_content, const rules::Players& players);
    GameState(const GameState& st) = default;
    ~GameState() = default;

    GameState* copy() const override;

    bool is_finished() const;

    int secret_number;
    bool secret_number_found;
    int round;
    std::unordered_map<uint32_t, int> player_guess_map;
};
