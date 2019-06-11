// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <memory>
#include <vector>

#include <rules/actions.hh>
#include <rules/game-state-history.hh>
#include <rules/game-state.hh>
#include <rules/player.hh>

#include "constant.hh"
#include "game_state.hh"

/*!
** The methods of this class are exported through 'interface.cc'
** to be called by the clients
*/
class Api
{
public:
    Api(std::unique_ptr<GameState> game_state, rules::Player_sptr player);
    virtual ~Api() {}

    const rules::Player_sptr player() const { return player_; }
    void player_set(rules::Player_sptr player) { player_ = player; }

    void apply_action(rules::IAction_sptr action);
    rules::Actions* actions() { return &actions_; }

    GameState& game_state() { return *game_state_; }
    const GameState& game_state() const { return *game_state_; }
    void clear_old_game_states() { game_state_.clear_old_versions(); }

private:
    rules::GameStateHistory<GameState> game_state_;
    rules::Player_sptr player_;
    rules::Actions actions_;

public:
    /// Play at the given position
    error play(position pos);

    /// Returns your team number
    int my_team();

    /// Returns the TicTacToe board
    std::vector<int> board();

    /// Cancels the last played action
    bool cancel();
};
