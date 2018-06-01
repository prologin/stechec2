#include "actions.hh"

int ActionPlay::check(const GameState* st) const
{
    if (!st->is_valid_cell(pos_))
        return OUT_OF_BOUNDS;
    if (st->get_cell(pos_) != st->NO_PLAYER)
        return ALREADY_OCCUPIED;

    return OK;
}

void ActionPlay::apply_on(GameState* st) const
{
    st->set_cell(pos_, player_);
}
