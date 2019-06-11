// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include <stdlib.h>

#include <memory>

#include "actions.hh"
#include "api.hh"

// global used in interface.cc
Api* api;

Api::Api(std::unique_ptr<GameState> game_state, rules::Player_sptr player)
    : rules::Api<GameState>(std::move(game_state), player)
{
    api = this;
}

/// Play at the given position
error Api::play(position pos)
{
    auto action = std::make_shared<ActionPlay>(pos, player_->id);

    error err;
    if ((err = static_cast<error>(game_state_check(action))) != OK)
        return err;

    actions_.add(action);
    game_state_save();
    game_state_apply(action);
    return OK;
}

/// Returns your team number
int Api::my_team()
{
    return player_->id;
}

/// Returns the TicTacToe board
std::vector<int> Api::board()
{
    return game_state().get_board();
}
