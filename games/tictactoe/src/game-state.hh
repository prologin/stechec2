#ifndef TICTACTOE_RULES_STATE_HH_
# define TICTACTOE_RULES_STATE_HH_

// Defines the game state.

# include <vector>
# include <ostream>
# include <rules/game-state.hh>

class GameState : public rules::GameState
{
public:
    GameState();
    virtual ~GameState();

    virtual rules::GameState* copy() const;

    std::vector<int>& board() { return board_; }
    const std::vector<int>& board() const { return board_; }

protected:
    // Copy constructor
    GameState(const GameState& gs) : rules::GameState(gs), board_(gs.board_) {}

    std::vector<int> board_;
};

std::ostream& operator<<(std::ostream& out, const GameState& gs);

#endif // !TICTACTOE_RULES_STATE_HH_
