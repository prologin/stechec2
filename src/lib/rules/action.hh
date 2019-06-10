// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <list>
#include <memory>
#include <utils/buffer.hh>

#include "game-state.hh"

namespace rules {

// Interface to be implemented by all action types.
class IAction
{
public:
    virtual ~IAction() {}

    // Check if the action can be applied to the state. Returns 0 if there is
    // no error, or a rule dependent error value if the action can't be
    // applied.
    virtual int check(const GameState* st) const = 0;

    // Copies the current state, returns a new state with the action applied
    // and add the applied action to the given list.
    GameState* apply(GameState* st) const;

    // Handles serialization and deserialization of the Action object to a
    // buffer.
    virtual void handle_buffer(utils::Buffer& buf) = 0;

    // Get the id of the action, necessaryr for multiple actions
    // serialization/unserialization
    virtual uint32_t id() const = 0;

    // Get the player id, which is useful for not applying an action twice
    virtual uint32_t player_id() const = 0;

private:
    // Only applies the action to a given state, without handling action
    // cancellation.
    virtual void apply_on(GameState* st) const = 0;
};

// A template that can be used to implement actions in an easier way without
// dynamic_casts everywhere.
// It's kind of ugly, but it allows a lot more genericity in the codebase.
template <typename TState>
class Action : public IAction
{
public:
    virtual int check(const TState* st) const = 0;
    int check(const GameState* st) const override
    {
        return check(dynamic_cast<const TState*>(st));
    }

    TState* apply(GameState* st) const
    {
        return dynamic_cast<TState*>(IAction::apply(st));
    }

protected:
    virtual void apply_on(TState* st) const = 0;
    void apply_on(GameState* st) const override
    {
        apply_on(dynamic_cast<TState*>(st));
    }
};

typedef std::shared_ptr<IAction> IAction_sptr;
typedef std::list<IAction_sptr> IActionList;
typedef IAction* (*f_action_factory)();

} // namespace rules
