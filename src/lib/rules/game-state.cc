// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "game-state.hh"

namespace rules {

GameState::~GameState()
{
    delete old_version_;
}

void GameState::clear_old_version()
{
    delete old_version_;
    old_version_ = nullptr;
}

} // namespace rules
