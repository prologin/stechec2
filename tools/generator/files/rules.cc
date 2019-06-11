// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "rules.hh"

#include <memory>
#include <utility>

#include "actions.hh"

Rules::Rules(const rules::Options opt)
    : !!rules_cc_inheritance!! sandbox_(opt.time)
{
    if (!opt.champion_lib.empty())
    {
        champion_dll_ = std::make_unique<utils::DLL>(opt.champion_lib);

        // FIXME: register user functions
    }

    auto game_state = std::make_unique<GameState>(opt.players);
    api_ = std::make_unique<Api>(std::move(game_state), opt.player);
    register_actions();
}

// @@GEN_HERE@@
