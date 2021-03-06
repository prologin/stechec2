// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2015 Association Prologin <association@prologin.org>
#pragma once

#include <rules/game-state.hh>
#include <rules/player.hh>

class GameState final : public rules::GameState
{
public:
    // FIXME
    // additional parameters? for instance map
    GameState(rules::Players_sptr players);
    GameState(const GameState& st);
    ~GameState();

    GameState* copy() const override;

private:
    rules::Players_sptr players_;
};
