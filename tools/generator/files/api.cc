// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "api.hh"

#include <stdlib.h>

#include "actions.hh"

// global used in interface.cc
Api* api;

Api::Api(std::unique_ptr<GameState> game_state, rules::Player_sptr player)
  : rules::Api<GameState>(std::move(game_state), player_(player)
{
    api = this;
}

// @@GEN_HERE@@
