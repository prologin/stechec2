// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include "action.hh"

namespace rules {

class GameState
{
public:
    GameState() = default;
    virtual ~GameState() = default;

    // Explicit copy of the game state
    virtual GameState* copy() const = 0;

    int check(IAction_sptr action) const { return action->check(*this); }
    void apply(IAction_sptr action) { action->apply(*this); }

protected:
    // Protected to be called by copy()
    GameState(const GameState&) = default;
};

} // namespace rules
