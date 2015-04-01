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
        Api(rules::GameState* game_state, rules::Player_sptr player);
        virtual ~Api() { }

        const GameState* game_state() const;
        GameState* game_state();
        void game_state_set(rules::GameState* gs);

    private:
        rules::GameState* game_state_;
        rules::Player_sptr player_;

    public:

        // @@GEN_HERE@@
};


#endif /* !API_HH_ */
