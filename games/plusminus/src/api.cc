// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "api.hh"

#include <stdlib.h>

#include "actions.hh"

// global used in interface.cc
Api* api;

Api::Api(std::unique_ptr<GameState> game_state, rules::Player_sptr player)
    : rules::Api<GameState, error>(std::move(game_state), player)
{
    api = this;
}

/// Returns your team number
int Api::my_team()
{
    return player_->id;
}

/// Returns -1 if you are below the number, 1 if you are above, 0 if you found
/// it. Returns 0 on the first turn.
int Api::result()
{
    return game_state().player_guess_map[player_->id];
}
