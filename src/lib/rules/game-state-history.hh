// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "game-state.hh"

namespace rules
{

// GameStateHistory<GameState> holds a GameState instance, can save it to a
// stack and allows restoring previous versions.
template <typename GameState>
class GameStateHistory
{
    static_assert(std::is_base_of<rules::GameState, GameState>::value,
                  "GameState not derived from rules::GameState");

public:
    GameStateHistory(std::shared_ptr<GameState> current)
        : current_(std::move(current))
    {
    }

    GameState& operator*()
    {
        return *current_.get();
    }
    const GameState& operator*() const
    {
        return *current_.get();
    }
    GameState* operator->()
    {
        return current_.get();
    }
    const GameState* operator->() const
    {
        return current_.get();
    }

    // Copies current GameState and stores it in the version history
    void save()
    {
        versions_.emplace_back(current_->copy());
    }
    // Returns true if there is a restorable GameState in the version history
    bool can_cancel() const
    {
        return !versions_.empty();
    }
    // Restores most recent version of the GameState
    void cancel()
    {
        assert(can_cancel());
        current_.swap(versions_.back());
        versions_.pop_back();
    }
    // Deletes previous copies of the GameState instance
    void clear_old_versions()
    {
        versions_.clear();
    }

private:
    std::shared_ptr<GameState> current_;
    std::vector<std::shared_ptr<GameState>> versions_;
};
} // namespace rules
