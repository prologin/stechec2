// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "rules.hh"

#include <fstream>
#include <memory>
#include <utility>

#include "actions.hh"

Rules::Rules(const rules::Options opt)
    : SynchronousRules(opt)
    , sandbox_(opt.time)
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

    if (opt.map_content.empty())
        FATAL("This game requires a map!");

    auto game_state = std::make_unique<GameState>(opt.map_content, opt.players);
    api_ = std::make_unique<Api>(std::move(game_state), opt.player);
    register_actions();
}

void Rules::register_actions()
{
    api_->actions()->register_action(
        ID_ACTION_GUESS, []() { return std::make_unique<ActionGuess>(); });
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
        FATAL("init_game: timeout");
    }
}

void Rules::at_player_end(rules::ClientMessenger_sptr)
{
    try
    {
        sandbox_.execute(champion_end_game_);
    }
    catch (utils::SandboxTimeout&)
    {
        FATAL("end_game: timeout");
    }
}

void Rules::player_turn()
{
    try
    {
        sandbox_.execute(champion_play_turn_);
    }
    catch (utils::SandboxTimeout&)
    {
        FATAL("play_turn: timeout");
    }
}

void Rules::end_of_round()
{
    api_->game_state().round += 1;
}

void Rules::at_end()
{
    for (const auto& p : players_)
    {
        auto guess = api_->game_state().player_guess_map[p->id];
        if (guess == 0)
            p->score = 1;
    }
}

bool Rules::is_finished()
{
    return api_->game_state().is_finished();
}
