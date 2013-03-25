#ifndef LIB_RULES_RULES_HH_
# define LIB_RULES_RULES_HH_

# include <utils/log.hh>

# include "options.hh"
# include "player.hh"
# include "actions.hh"
# include "action.hh"
# include "server-messenger.hh"
# include "client-messenger.hh"

namespace rules {

// Interface of the rules
class Rules
{
public:
    explicit Rules(const Options opt);

    // Function executed at the start of the game
    virtual void at_start() = 0;

    // Function executed at the end of the game
    virtual void at_end() = 0;

    // Check whether the game is over
    virtual bool is_finished() = 0;

    // Get the actions structure with registered actions
    virtual Actions* get_actions() = 0;

    // Apply an action to the game state
    virtual void apply_action(const IAction_sptr& action) = 0;

    // Player's turn: call the champion within a sandbox
    virtual void player_turn() = 0;

    // Spectator's turn: call the champion wihout any sandbox
    virtual void spectator_turn() = 0;

    // Called once every player has played
    virtual void end_of_turn() = 0;

    virtual void client_loop(ClientMessenger_sptr msgr) = 0;
    virtual void spectator_loop(ClientMessenger_sptr msgr) = 0;
    virtual void server_loop(ServerMessenger_sptr msgr) = 0;

protected:
    bool is_spectator(uint32_t id);

    Options opt_;

    Players_sptr players_;
    Players_sptr spectators_;

    int timeout_;
};

class SynchronousRules : public Rules
{
public:
    explicit SynchronousRules(const Options opt);

    virtual void client_loop(ClientMessenger_sptr msgr) final;
    virtual void spectator_loop(ClientMessenger_sptr msgr) final;
    virtual void server_loop(ServerMessenger_sptr msgr) final;
};

class TurnBasedRules : public Rules
{
public:
    explicit TurnBasedRules(const Options opt);

    // Called each time a player has finished its turn
    virtual void end_of_player_turn(uint32_t player_id) = 0;

    virtual void client_loop(ClientMessenger_sptr msgr) final;
    virtual void spectator_loop(ClientMessenger_sptr msgr) final;
    virtual void server_loop(ServerMessenger_sptr msgr) final;
};

} // namespace rules

#endif // LIB_RULES_RULES_HH_
