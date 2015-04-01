// FIXME License notice

#include "game_state.hh"

GameState::GameState(rules::Players_sptr players)
    : rules::GameState(),
      players_(players)
{
    // FIXME
}

GameState::GameState(const GameState& st)
    : rules::GameState(st)
    , players_(st.players_)
{
    // FIXME
}

rules::GameState* GameState::copy() const
{
    return new GameState(*this);
}

GameState::~GameState()
{
    // FIXME
}
