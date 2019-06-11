// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2013 Association Prologin <association@prologin.org>
#pragma once

#include <utils/log.hh>

#include "action.hh"
#include "actions.hh"
#include "client-messenger.hh"
#include "options.hh"
#include "player.hh"
#include "server-messenger.hh"

namespace rules {

const int max_consecutive_timeout = 3;

// Interface of the rules
class Rules
{
public:
    explicit Rules(const Options opt);
    virtual ~Rules() {}

    /* Pure virtual methods */

    virtual void player_loop(ClientMessenger_sptr msgr) = 0;
    virtual void spectator_loop(ClientMessenger_sptr msgr) = 0;
    virtual void server_loop(ServerMessenger_sptr msgr) = 0;

    // Get the actions structure with registered actions
    virtual Actions* get_actions() = 0;

    // Apply an action to the game state
    virtual void apply_action(const IAction_sptr& action) = 0;

    // Check whether the game is over
    virtual bool is_finished() = 0;

    /* Placeholders */

    // Function executed at the start of the game
    virtual void at_start() {}
    virtual void at_player_start(ClientMessenger_sptr) {}
    virtual void at_spectator_start(ClientMessenger_sptr) {}
    virtual void at_server_start(ServerMessenger_sptr) {}

    // Function executed at the end of the game
    virtual void at_end() {}
    virtual void at_player_end(ClientMessenger_sptr) {}
    virtual void at_spectator_end(ClientMessenger_sptr) {}
    virtual void at_server_end(ServerMessenger_sptr) {}

    // Player's turn: call the champion within a sandbox
    virtual void player_turn() {}

    // Spectator's turn: call the champion wihout any sandbox
    virtual void spectator_turn() {}

    // Called before / after every player has played
    virtual void start_of_round() {}
    virtual void end_of_round() {}

    // Called at every turn by the server to dump the gamestate in the dumpfile
    virtual void dump_state(std::ostream&)
    {
        FATAL("dump_state() virtual function not implemented in the rules "
              "but the server was launched with the `--dump` option.");
    }

protected:
    bool is_spectator(uint32_t id);

    void dump_state_stream()
    {
        if (opt_.dump_stream)
            dump_state(*opt_.dump_stream);
    }

    Options opt_;

    Players_sptr players_;
    Players_sptr spectators_;

    int timeout_;
};

class SynchronousRules : public Rules
{
public:
    explicit SynchronousRules(const Options opt);
    ~SynchronousRules() override {}

    void player_loop(ClientMessenger_sptr msgr) final;
    void spectator_loop(ClientMessenger_sptr msgr) final;
    void server_loop(ServerMessenger_sptr msgr) final;
};

class TurnBasedRules : public Rules
{
public:
    explicit TurnBasedRules(const Options opt);
    ~TurnBasedRules() override {}

    // Called each time a client starts its turn
    virtual void start_of_turn(uint32_t) {}
    virtual void start_of_player_turn(uint32_t) {}
    virtual void start_of_spectator_turn(uint32_t) {}

    // Called each time a client has finished its turn
    virtual void end_of_turn(uint32_t) {}
    virtual void end_of_player_turn(uint32_t) {}
    virtual void end_of_spectator_turn(uint32_t) {}

    void player_loop(ClientMessenger_sptr msgr) final;
    void spectator_loop(ClientMessenger_sptr msgr) final;
    void server_loop(ServerMessenger_sptr msgr) final;
};

} // namespace rules
