#include "game-state.hh"

GameState::GameState() : board_({-1, -1, -1,
                                 -1, -1, -1,
                                 -1, -1, -1})
{
}

GameState::~GameState()
{
}

rules::GameState* GameState::copy() const
{
    return new GameState(*this);
}
