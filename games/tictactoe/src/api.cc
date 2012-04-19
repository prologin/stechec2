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

#include <stdlib.h>

#include "api.hh"

// global used in interface.cc
Api* api;

Api::Api(rules::GameState* game_state, rules::Player* player)
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
  // TODO
  abort();
}

///
// Returns the TicTacToe board
//
std::vector<int> Api::board()
{
  // TODO
  abort();
}

///
// Play at the given position
//
error Api::play(int x, int y)
{
  // TODO
  abort();
}

///
// Cancels the last played action
//
bool Api::cancel()
{
  // TODO
  abort();
}

///
// Affiche le contenu d'une valeur de type error
//

