#ifndef TICTACTOE_RULES_ACTION_PLAY_HH_
# define TICTACTOE_RULES_ACTION_PLAY_HH_

# include <rules/action.hh>

# include "game-state.hh"

class ActionPlay : public rules::Action<GameState>
{
public:
    ActionPlay(int x = -1, int y = -1, int player = -1);

    virtual int check(const GameState* st) const;
    virtual void handle_buffer(utils::Buffer& buf);

protected:
    virtual void apply_on(GameState* st) const;

    // Coordinates of the play.
    int x_, y_;

    // Player id
    int player_;
};

#endif // !TICTACTOE_RULES_ACTION_PLAY_HH_
