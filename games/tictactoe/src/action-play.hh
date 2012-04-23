#ifndef TICTACTOE_ACTION_PLAY_HH_
# define TICTACTOE_ACTION_PLAY_HH_

# include <cstdint>
# include <rules/action.hh>

# include "game-state.hh"

class ActionPlay : public rules::Action<GameState>
{
public:
    ActionPlay(int x = -1, int y = -1, int player = -1);

    virtual int check(const GameState* st) const;
    virtual void handle_buffer(utils::Buffer& buf);

    uint32_t player_id() const
        { return player_; }
    uint32_t id() const
        { return id_; }

protected:
    virtual void apply_on(GameState* st) const;

    // Coordinates of the play.
    int32_t x_, y_;

    // Player id
    int32_t player_;

    uint32_t id_;
};

#endif // !TICTACTOE_ACTION_PLAY_HH_
