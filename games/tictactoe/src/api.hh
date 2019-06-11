// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <vector>

#include <rules/api.hh>
#include <rules/player.hh>

#include "constant.hh"
#include "game_state.hh"

class Api;

extern Api* api;

/*!
** The methods of this class are exported through 'interface.cc'
** to be called by the clients
*/
class Api : public rules::Api<GameState>
{
public:
    Api(std::unique_ptr<GameState> game_state, rules::Player_sptr player)
        : rules::Api<GameState>(std::move(game_state), player)
    {
        api = this;
    }

    /// Play at the given position
    error play(position pos);

    /// Returns your team number
    int my_team();

    /// Returns the TicTacToe board
    std::vector<int> board();

    /// Cancels the last played action
    bool cancel();
};
