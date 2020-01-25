// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "actions.hh"
#include "rules.hh"

Rules::Rules(const rules::Options opt) : TurnBasedRules(opt), sandbox_(opt.time)
{
    if (!opt.champion_lib.empty())
    {
        champion_dll_ = std::make_unique<utils::DLL>(opt.champion_lib);

        champion_init_game_ =
            champion_dll_->get<f_champion_init_game>("init_game");
        champion_play_turn_ =
            champion_dll_->get<f_champion_play_turn>("play_turn");
        champion_end_game_ =
            champion_dll_->get<f_champion_end_game>("end_game");
    }

    auto game_state = std::make_unique<GameState>(opt.players);
    api_ = std::make_unique<Api>(std::move(game_state), opt.player);
    register_actions();
}

void Rules::register_actions()
{
    api_->actions()->register_action(
        ID_ACTION_PLAY, []() { return std::make_unique<ActionPlay>(); });
}

rules::Actions* Rules::get_actions()
{
    return api_->actions();
}

void Rules::apply_action(const rules::IAction& action)
{
    // When receiving an action, the API should have already checked that it
    // is valid. We recheck that for the current gamestate here to avoid weird
    // client/server desynchronizations and make sure the gamestate is always
    // consistent across the clients and the server.

    int err = api_->game_state_check(action);
    if (err)
        FATAL("Synchronization error: received action %d from player %d, but "
              "check() on current gamestate returned %d.",
              action.id(), action.player_id(), err);
    api_->game_state_apply(action);
}

void Rules::at_player_start(rules::ClientMessenger_sptr)
{
    try
    {
        sandbox_.execute(champion_init_game_);
    }
    catch (utils::SandboxTimeout&)
    {
        FATAL("player_start: timeout");
    }
}

void Rules::at_spectator_start(rules::ClientMessenger_sptr)
{
    champion_init_game_();
}

void Rules::at_player_end(rules::ClientMessenger_sptr)
{
    try
    {
        sandbox_.execute(champion_end_game_);
    }
    catch (utils::SandboxTimeout&)
    {
        FATAL("player_end: timeout");
    }
}

void Rules::at_spectator_end(rules::ClientMessenger_sptr)
{
    champion_end_game_();
}

void Rules::player_turn()
{
    try
    {
        sandbox_.execute(champion_play_turn_);
    }
    catch (utils::SandboxTimeout&)
    {
        FATAL("player_turn: timeout");
    }
}

void Rules::spectator_turn()
{
    champion_play_turn_();
}

void Rules::start_of_player_turn(unsigned int player_id)
{
    api_->game_state().set_player_turn(player_id, true);
}

void Rules::end_of_player_turn(unsigned int /* player_id */)
{
    // Clear the list of game states at the end of each turn (half-round)
    // We need the linked list of game states only for undo and history,
    // therefore old states are not needed anymore after the turn ends.
    api_->clear_old_game_states();
    api_->game_state().compute_scores();
}

bool Rules::is_finished()
{
    return api_->game_state().is_finished();
}

GameState& Rules::game_state()
{
    return api_->game_state();
}

const GameState& Rules::game_state() const
{
    return api_->game_state();
}
