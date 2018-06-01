/*
** Stechec project is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** The complete GNU General Public Licence Notice can be found as the
** `NOTICE' file in the root directory.
**
** Copyright (C) 2018 Prologin
*/

#include <stdlib.h>

#include "actions.hh"
#include "api.hh"

// global used in interface.cc
Api* api;

Api::Api(GameState* game_state, rules::Player_sptr player)
    : game_state_(game_state)
    , player_(player)
{
    api = this;
}

/// Play at the given position
error Api::play(position pos)
{
    rules::IAction_sptr action(new ActionPlay(pos, player_->id));

    error err;
    if ((err = static_cast<error>(action->check(game_state_))) != OK)
        return err;

    actions_.add(action);
    game_state_set(action->apply(game_state_));
    return OK;
}

/// Returns your team number
int Api::my_team()
{
    // TODO
    abort();
}

/// Returns the TicTacToe board
std::vector<int> Api::board()
{
    // TODO
    abort();
}

/// Cancels the last played action
bool Api::cancel()
{
    // TODO
    abort();
}
