#include "actions.hh"

int ActionGuess::check(const GameState& st) const
{
    if (guess_ < 0 || guess_ > 100)
        return OUT_OF_BOUNDS;
    return OK;
}

void ActionGuess::apply_on(GameState* st) const
{
    int result;
    if (guess_ == st->secret_number)
    {
        st->secret_number_found = true;
        result = 0;
    }
    else if (guess_ < st->secret_number)
        result = -1;
    else
        result = 1;

    st->player_guess_map[player_id_] = result;
}
