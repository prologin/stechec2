/*
** Stechec project is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** The complete GNU General Public Licence Notice can be found as the
** `NOTICE' file in the root directory.
**
** Copyright (C) 2012 Prologin
*/

#include "api.hh"

#include <stdlib.h>
#include <rules/game-state.hh>

#include "action-play.hh"

// global used in interface.cc
Api* api;

Api::Api(GameState* game_state, rules::Player* player)
    : game_state_(game_state),
      player_(player)
{
  api = this;
}

///
// Returns your team number
//
int Api::my_team()
{
    return player_->id;
}

///
// Returns the TicTacToe board
//
std::vector<int> Api::board()
{
    return game_state_->board();
}

///
// Play at the given position
//
error Api::play(int x, int y)
{
    ActionPlay action(x, y, player_->id);

    error err;
    if ((err = static_cast<error>(action.check(game_state_))) != OK)
        return err;

    game_state_ = action.apply(game_state_);

    return OK;
}

///
// Cancels the last played action
//
bool Api::cancel()
{
    if (!game_state_->can_cancel())
        return false;

    game_state_ = rules::cancel(game_state_);

    return true;
}
