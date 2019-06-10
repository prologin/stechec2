// SPDX-License-Identifier: GPL-2.0-or-later
#include "action.hh"

namespace rules {

GameState* IAction::apply(GameState* st) const
{
    // Copy the old state and link the new version to the old version.
    GameState* new_st = st->copy();
    new_st->set_old_version(st);

    // Apply the action on the new state.
    this->apply_on(new_st);

    return new_st;
}

} // namespace rules
