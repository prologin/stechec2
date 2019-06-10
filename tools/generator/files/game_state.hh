// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <rules/game-state.hh>
#include <rules/player.hh>

class GameState : public rules::GameState
{
public:
    // FIXME
    // additional parameters? for instance map
    GameState(rules::Players_sptr players);
    GameState(const GameState& st);
    rules::GameState* copy() const override;
    ~GameState();

private:
    rules::Players_sptr players_;
};
