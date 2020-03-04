// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include <cstdlib>
#include <memory>

#include <rules/client-messenger.hh>
#include <rules/config.hh>
#include <rules/server-messenger.hh>
#include <utils/log.hh>

#include "rules.hh"

// Forward decls
namespace rules
{
struct Options;
}

static Rules* rules_;

extern "C"
{
    void rules_config(rules::Config* cfg)
    {
        cfg->name = "tictactoe";
        cfg->player_count = 2;
    }

    void rules_init(const rules::Options& opt)
    {
        utils::Logger::get().level() = (utils::Logger::DisplayLevel)opt.verbose;
        rules_ = new Rules(opt);
    }

    void rules_result()
    {
        delete rules_;
    }

    void player_loop(rules::ClientMessenger_sptr msgr)
    {
        rules_->player_loop(msgr);
    }

    void replay_loop(rules::ReplayMessenger_sptr msgr)
    {
        rules_->replay_loop(msgr);
    }

    void server_loop(rules::ServerMessenger_sptr msgr)
    {
        rules_->server_loop(msgr);
    }

    void spectator_loop(rules::ClientMessenger_sptr msgr)
    {
        rules_->spectator_loop(msgr);
    }

} // extern "C"
