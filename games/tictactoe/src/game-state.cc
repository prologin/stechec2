#include "game-state.hh"

#include <utils/log.hh>

GameState::GameState() : board_({NO_PLAYER, NO_PLAYER, NO_PLAYER,
                                 NO_PLAYER, NO_PLAYER, NO_PLAYER,
                                 NO_PLAYER, NO_PLAYER, NO_PLAYER})
{
}

GameState::~GameState()
{
}

rules::GameState* GameState::copy() const
{
    return new GameState(*this);
}

std::ostream& operator<<(std::ostream& out, const GameState& gs)
{
    for (size_t i = 0; i < gs.board().size(); ++i)
    {
        if (i % 3 == 0 && i != 0)
            out << "\n";
        out << gs.board().at(i);
    }

    out << std::endl;

    return out;
}
