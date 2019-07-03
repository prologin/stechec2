// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <memory>
#include <vector>

#include <rules/actions.hh>
#include <rules/api.hh>
#include <rules/player.hh>

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

    // @@GEN_HERE@@
};
