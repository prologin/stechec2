// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <functional>
#include <list>
#include <memory>

#include <utils/buffer.hh>

#include "game-state.hh"

namespace rules {

class GameState;

// Interface to be implemented by all action types.
class IAction
{
public:
    virtual ~IAction() {}

    // Check if the action can be applied to the state. Returns 0 if there is
    // no error, or a rule dependent error value if the action can't be
    // applied.
    virtual int check(const GameState& st) const = 0;

    // Apply action to game state.
    virtual void apply(GameState& st) const = 0;

    // Handles serialization and deserialization of the Action object to a
    // buffer.
    virtual void handle_buffer(utils::Buffer& buf) = 0;

    // Get the id of the action, necessary for multiple actions
    // serialization/unserialization
    virtual uint32_t id() const = 0;

    // Get the player id, which is useful for not applying an action twice
    virtual uint32_t player_id() const = 0;
};

// A template that can be used to implement actions in an easier way without
// dynamic_casts everywhere.
// It's kind of ugly, but it allows a lot more genericity in the codebase.
template <typename TState>
class Action : public IAction
{
public:
    virtual int check(const TState& st) const = 0;
    int check(const GameState& st) const override
    {
        return check(dynamic_cast<const TState&>(st));
    }

    virtual void apply(TState& st) const = 0;
    void apply(GameState& st) const override
    {
        apply(dynamic_cast<TState&>(st));
    }
};

using IAction_sptr = std::shared_ptr<IAction>;
using IActionList = std::list<IAction_sptr>;
using ActionFactory = std::function<IAction*()>;

} // namespace rules
