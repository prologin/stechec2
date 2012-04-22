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
# include <rules/player-actions.hh>

# include "game-state.hh"
# include "constant.hh"

/*!
** Method of this call are called by the candidat, throught 'interface.cc'
*/
class Api
{

public:
    Api(GameState* game_state, rules::Player_sptr player);
    virtual ~Api() { }

    const rules::Player_sptr player() const
        { return player_; }
    void player_set(rules::Player_sptr player)
        { player_ = player; }

    rules::PlayerActions* player_actions()
        { return &player_actions_; }

    const GameState* game_state() const
        { return game_state_; }

private:
    GameState* game_state_;
    rules::Player_sptr player_;
    rules::PlayerActions player_actions_;

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

};


#endif /* !API_HH_ */
