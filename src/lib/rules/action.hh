// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <utils/buffer.hh>

namespace rules
{

class GameState;

// Interface to be implemented by all action types.
class IAction
{
public:
    virtual ~IAction() = default;

    // Check if the action can be applied to the state. Returns 0 if there is
    // no error, or a rule dependent error value if the action can't be
    // applied.
    virtual int check(const GameState& st) const = 0;

    // Apply action to game state.
    virtual void apply(GameState* st) const = 0;

    // Outputs to a stream the json dump of the action.
    virtual void dump_json(const GameState& st) const;

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
    static_assert(std::is_base_of<rules::GameState, TState>::value,
                  "TState not derived from rules::GameState");

public:
    virtual int check(const TState& st) const = 0;
    int check(const GameState& st) const override
    {
        return check(static_cast<const TState&>(st));
    }

    virtual void apply_on(TState* st) const = 0;
    void apply(GameState* st) const override
    {
        apply_on(static_cast<TState*>(st));
    }
    void apply(std::unique_ptr<TState>& st) const
    {
        apply_on(st.get());
    }

    // Outputs to a stream the json dump of the action.
    virtual void dump_json(const TState& st) const;
    void dump_json(const GameState& st) const override
    {
        return dump_json(static_cast<const TState&>(st));
    }
};

} // namespace rules
