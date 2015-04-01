/*
** Stechec project is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** The complete GNU General Public Licence Notice can be found as the
** `NOTICE' file in the root directory.
**
** Copyright (C) !!year!! !!provider!!
*/

#include <stdlib.h>

#include "api.hh"

// global used in interface.cc
Api* api;

Api::Api(rules::GameState* game_state, rules::Player_sptr player)
    : game_state_(game_state),
      player_(player)
{
  api = this;
}

const GameState* game_state() const {
    return game_state_;
}
GameState* game_state() {
    return game_state_;
}

void game_state_set(rules::GameState* gs) {
    game_state_ = dynamic_cast<GameState*>(gs);
}


// @@GEN_HERE@@
