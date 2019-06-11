// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2018 Association Prologin <association@prologin.org>
#pragma once

#include <rules/action.hh>

#include "actions.hh"
#include "constant.hh"
#include "game_state.hh"

class ActionPlay : public rules::Action<GameState>
{
public:
    ActionPlay(position pos, int player_id) : pos_(pos), player_id_(player_id)
    {}
    ActionPlay() {} // for register_action()

    int check(const GameState* st) const override;
    void apply_on(GameState* st) const override;

    void handle_buffer(utils::Buffer& buf) override
    {
        buf.handle(pos_);
        buf.handle(player_id_);
    }

    uint32_t player_id() const override { return player_id_; };
    uint32_t id() const override { return ID_ACTION_PLAY; }

private:
    position pos_;
    int player_id_;
};
