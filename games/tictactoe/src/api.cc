// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include <stdlib.h>

#include <memory>

#include "actions.hh"
#include "api.hh"

// global used in interface.cc
Api* api;

/// Play at the given position
error Api::play(position pos)
{
    auto action = std::make_shared<ActionPlay>(pos, player_->id);

    error err;
    if ((err = static_cast<error>(action->check(game_state()))) != OK)
        return err;

    actions_.add(action);
    game_state_.save();
    action->apply(game_state());
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

/// Cancels the last played action
bool Api::cancel()
{
    if (!game_state_.can_cancel())
        return false;
    actions_.cancel();
    game_state_.cancel();
    return true;
}
