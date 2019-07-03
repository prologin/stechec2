/// This file has been generated, if you wish to
/// modify it in a permanent way, please refer
/// to the script file : gen/generator_cxx.rb

#include "prologin.hh"

/// Function called at the start of the game
void init_game() {}

/// Called when this is your turn to play
void play_turn()
{
    if (my_team() == 1)
        guess(42);
    else
        guess(13);
}

/// Function called at the end of the game
void end_game() {}
