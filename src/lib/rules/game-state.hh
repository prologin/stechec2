// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

namespace rules {

class GameState
{
public:
    GameState() = default;
    virtual ~GameState() = default;

    // Explicit copy of the game state
    virtual GameState* copy() const = 0;

protected:
    // Protected to be called by copy()
    GameState(const GameState&) = default;
};

} // namespace rules
