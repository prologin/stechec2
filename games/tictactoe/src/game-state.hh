#ifndef TICTACTOE_RULES_STATE_HH_
# define TICTACTOE_RULES_STATE_HH_

// Defines the game state.

# include <rules/game-state.hh>
# include <vector>

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

#endif // !TICTACTOE_RULES_STATE_HH_
