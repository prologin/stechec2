///
// This file has been generated, if you wish to
// modify it in a permanent way, please refer
// to the script file : gen/generator_cxx.rb
//

#include "prologin.hh"

#include <vector>
#include <iostream>

///
// Function called at the start of the game
//
void init_game()
{
  // fonction a completer
}

///
// Called when this is your turn to play
//
void play_turn()
{
    std::vector<int> b = board();

    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
            std::cout << b[x * 3 + y];

        std::cout << std::endl;
    }
}

///
// Function called at the end of the game
//
void end_game()
{
  // fonction a completer
}

