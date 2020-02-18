// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include "action.hh"
#include "player.hh"

namespace rules
{

class GameState
{
public:
    GameState()
        : players_{}
    {
    }
    GameState(const Players players)
        : players_{players}
    {
    }
    virtual ~GameState() = default;

    // Explicit copy of the game state
    // TODO(halfr): add default implem and return a std::unique_ptr
    virtual GameState* copy() const = 0;

    int check(const IAction& action) const { return action.check(*this); }
    void apply(const IAction& action) { action.apply(this); }

protected:
    // Protected to be called by copy()
    GameState(const GameState&) = default;

    const Players players_;
};

} // namespace rules
