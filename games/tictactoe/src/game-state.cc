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

int GameState::winner() const
{
    // Check lines
    for (int line = 0; line < 3; ++line)
    {
        if (cell(0, line) != NO_PLAYER &&
                cell(0, line) == cell(1, line) &&
                cell(0, line) == cell(2, line))
            return cell(0, line);
    }

    // Check columns
    for (int col = 0; col < 3; ++col)
    {
        if (cell(col, 0) != NO_PLAYER &&
                cell(col, 0) == cell(col, 1) &&
                cell(col, 0) == cell(col, 2))
            return cell(col, 0);
    }

    // Check diagonals
    if (cell(0, 0) != NO_PLAYER &&
            cell(0, 0) == cell(1, 1) &&
            cell(0, 0) == cell(2, 2))
        return cell(0, 0);

    if (cell(0, 2) != NO_PLAYER &&
            cell(0, 2) == cell(1, 1) &&
            cell(0, 2) == cell(2, 0))
        return cell(0, 2);

    return NO_PLAYER;
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
