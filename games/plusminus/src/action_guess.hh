#ifndef ACTION_GUESS_HH
#define ACTION_GUESS_HH

#include <rules/action.hh>

#include "actions.hh"
#include "constant.hh"
#include "game_state.hh"

class ActionGuess : public rules::Action<GameState>
{
public:
    ActionGuess(int guess, int player_id) : guess_(guess), player_id_(player_id)
    {}
    ActionGuess() {} // for register_action()

    int check(const GameState& st) const override;
    void apply_on(GameState* st) const override;

    void handle_buffer(utils::Buffer& buf) override
    {
        buf.handle(guess_);
        buf.handle(player_id_);
    }

    uint32_t player_id() const override { return player_id_; };
    uint32_t id() const override { return ID_ACTION_GUESS; }

private:
    int guess_;
    int player_id_;
};

#endif // !ACTION_GUESS_HH
