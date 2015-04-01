#ifndef TICTACTOE_RULES_STATE_HH_
# define TICTACTOE_RULES_STATE_HH_

// Defines the game state.

# include <vector>
# include <ostream>
# include <rules/game-state.hh>

class GameState : public rules::GameState
{
public:
    const int NO_PLAYER = -1;

    GameState();
    virtual ~GameState();

    virtual rules::GameState* copy() const;

    const std::vector<int>& board() const { return board_; }
    std::vector<int>& board() { return board_; }

    bool is_valid_cell(int x, int y) const
    {
        return (0 <= x && x < 3
                && 0 <= y && y < 3);
    }

    const int &cell(int x, int y) const
    {
        CHECK(is_valid_cell(x, y));
        return board_[3 * y + x];
    }
    int &cell(int x, int y) {
        CHECK(is_valid_cell(x, y));
        return board_[3 * y + x];
    }

    int winner() const;

protected:
    // Copy constructor
    GameState(const GameState& gs) : rules::GameState(gs), board_(gs.board_) {}

    std::vector<int> board_;
};

std::ostream& operator<<(std::ostream& out, const GameState& gs);

#endif // !TICTACTOE_RULES_STATE_HH_
