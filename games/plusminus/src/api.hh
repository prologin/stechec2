// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <memory>
#include <vector>

#include <rules/actions.hh>
#include <rules/api.hh>
#include <rules/player.hh>

#include "actions.hh"
#include "constant.hh"
#include "game_state.hh"

/*!
** The methods of this class are exported through 'interface.cc'
** to be called by the clients
*/
class Api final : public rules::Api<GameState, error>
{
public:
    Api(std::unique_ptr<GameState> game_state, rules::Player_sptr player);
    ~Api() {}

    /// Provide your guess
    ApiActionFunc<ActionGuess> guess{this};

    /// Returns your team number
    int my_team();

    /// Returns -1 if you are below the number, 1 if you are above, 0 if you /
    /// found it. Returns -2 when no valid guess has been received.
    int result();
};
