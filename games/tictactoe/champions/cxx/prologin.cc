#include "prologin.hh"

static int turn = 0;

void init_game() {}

void play_turn()
{
    if (my_team() % 2)
    {
        if (turn == 0)
            play({0, 0});
        else if (turn == 1)
            play({2, 0});
        else if (turn == 2)
            play({0, 1});
    }
    else
        play({1, turn});

    turn += 1;
}

void end_game() {}
