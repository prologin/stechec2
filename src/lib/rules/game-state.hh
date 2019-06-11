// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <utils/log.hh>

namespace rules {

// Interface of the game state.
class GameState
{
public:
    // Default constructor: no older version.
    GameState() : old_version_(nullptr) {}

    virtual ~GameState();

    void operator=(const rules::GameState&) = delete;

    // Copies this state and returns a new state object with the same data.
    virtual GameState* copy() const = 0;

    // Getter/Setter for the old version of the state.
    // See below for an explanation of the state versions.
    GameState* get_old_version() const { return old_version_; }
    void set_old_version(GameState* st) { old_version_ = st; }

    // Clear the old version of the state.
    // It should be used to avoid keeping useless old verson of the state.
    void clear_old_version();

    // Checks if the state has something to cancel.
    bool can_cancel() const { return old_version_ != nullptr; }

protected:
    // Copy constructor. Should not be called directly, use the "copy" virtual
    // member function.
    //
    // Note that this does not copy the "old version": having two objects
    // owning the old version would require refcounting to work properly during
    // object destruction. For this reason, we require the user to attach the
    // old version explicitly if needed.
    //
    // Most of the time, copying is done in order to use the copy as the new
    // version, so the old version is overwritten anyway. See Action::apply.
    GameState(const GameState&) : old_version_(nullptr) {}

    // The older version of this state.
    // Basically, each state is linked to its older version so that an action
    // can be simply cancelled by deleting the current version and reusing the
    // older version.
    GameState* old_version_;
};

// Cancels the last action that occurred on the state. External to the state
// so it can have a generic return type and delete the old version.
template <typename T>
T* cancel(T* current_version)
{
    CHECK(current_version->can_cancel());

    T* old_version = dynamic_cast<T*>(current_version->get_old_version());

    // Remove the link so that deleting current_version does not delete the
    // older version.
    current_version->set_old_version(0);
    delete current_version;

    return old_version;
}

} // namespace rules
