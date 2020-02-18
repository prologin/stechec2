// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include <stdlib.h>

#include <memory>

#include "actions.hh"
#include "api.hh"

// global used in interface.cc
Api* api;

Api::Api(std::unique_ptr<GameState> game_state,
         std::shared_ptr<rules::Player> player)
    : rules::Api<GameState, error>(std::move(game_state), player)
{
    api = this;
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
