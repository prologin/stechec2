// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <memory>

#include <rules/actions.hh>
#include <rules/client-messenger.hh>
#include <rules/options.hh>
#include <rules/player.hh>
#include <rules/rules.hh>
#include <rules/server-messenger.hh>
#include <utils/dll.hh>
#include <utils/sandbox.hh>

#include "api.hh"

using f_champion_init_game = void (*)();
using f_champion_play_turn = void (*)();
using f_champion_end_game = void (*)();

class Rules final : public rules::SynchronousRules
{
public:
    explicit Rules(const rules::Options opt);
    ~Rules() = default;

    rules::Actions* get_actions() override;
    void apply_action(const rules::IAction& action) override;

    void at_player_start(rules::ClientMessenger_sptr) override;
    void at_player_end(rules::ClientMessenger_sptr) override;

    void player_turn() override;

    void end_of_round() override;
    void at_end() override;

    bool is_finished() override;

protected:
    f_champion_init_game champion_init_game_;
    f_champion_play_turn champion_play_turn_;
    f_champion_end_game champion_end_game_;

private:
    void register_actions();

    std::unique_ptr<utils::DLL> champion_dll_;
    std::unique_ptr<Api> api_;
    utils::Sandbox sandbox_;
};
