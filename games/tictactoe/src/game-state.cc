#include "game-state.hh"

#include <utils/log.hh>

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
    DEBUG("GameState::copy");

    return new GameState(*this);
}
