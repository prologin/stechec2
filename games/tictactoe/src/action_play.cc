// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2018 Association Prologin <association@prologin.org>
#include "actions.hh"

int ActionPlay::check(const GameState& st) const
{
    if (!st.is_valid_cell(pos_))
        return OUT_OF_BOUNDS;
    if (st.get_cell(pos_) != st.NO_PLAYER)
        return ALREADY_OCCUPIED;
    if (!st.player_can_play(player_id_))
        return ALREADY_PLAYED;

    return OK;
}

void ActionPlay::apply_on(GameState* st) const
{
    st->set_cell(pos_, player_id_);
    st->set_player_can_play(player_id_, false);
}
