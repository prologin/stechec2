#include "game-state.hh"

namespace rules {

GameState::~GameState()
{
    delete old_version_;
}

} // namespace rules
