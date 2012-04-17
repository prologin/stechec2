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
# include <lib/rules/game-state.hh>

# include "Constant.hh"

/*!
** Method of this call are called by the candidat, throught 'interface.cc'
*/
class Api
{

public:
    Api(GameState* game_state, Player* player);
    virtual ~Api() { }

private:
    GameState* game_state_;
    Player* player_;

public:

    // @@GEN_HERE@@
};


#endif /* !API_HH_ */
