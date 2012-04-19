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

#ifndef API_HH_
# define API_HH_

# include <vector>
# include <rules/game-state.hh>
# include <rules/player.hh>

# include "constant.hh"

/*!
** Method of this call are called by the candidat, throught 'interface.cc'
*/
class Api
{

public:
    Api(rules::GameState* game_state, rules::Player* player);
    virtual ~Api() { }

private:
    rules::GameState* game_state_;
    rules::Player* player_;

public:

///
// Returns your team number
//
   int my_team();
///
// Returns the TicTacToe board
//
   std::vector<int> board();
///
// Play at the given position
//
   error play(int x, int y);
///
// Cancels the last played action
//
   bool cancel();
///
// Affiche le contenu d'une valeur de type error
//

};


#endif /* !API_HH_ */
