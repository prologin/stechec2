// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#pragma once

#include <memory>

#include "actions.hh"
#include "game-state-history.hh"

namespace rules {

template <typename GameState>
class Api
{
public:
    Api(std::unique_ptr<GameState> game_state, Player_sptr player)
        : game_state_(std::move(game_state)), player_(player)
    {}
    virtual ~Api() {}

    const Player_sptr player() const { return player_; }
    void player_set(Player_sptr player) { player_ = player; }

    void apply_action(IAction_sptr action);
    Actions* actions() { return &actions_; }

    GameState& game_state() { return *game_state_; }
    const GameState& game_state() const { return *game_state_; }
    void clear_old_game_states() { game_state_.clear_old_versions(); }

protected:
    GameStateHistory<GameState> game_state_;
    Player_sptr player_;
    Actions actions_;
};

} // namespace rules
