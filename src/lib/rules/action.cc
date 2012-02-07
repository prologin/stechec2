#include "action.hh"
#include "state.hh"

#include <cassert>

RulesState* Action::apply(RulesState* st) const
{
    // Copy the old state and link the new version to the old version.
    RulesState* new_st = st->copy();
    new_st->set_old_version(st);

    // Apply the action on the new state.
    this->apply_on(new_st);

    return new_st;
}

RulesState* Action::cancel(RulesState* st) const
{
    // Get the older version of the state.
    RulesState* old_st = st->get_old_version();
    assert(old_st != 0);

    // Unlink the old version so that deleting the new state does not delete
    // the old state.
    st->set_old_version(0);
    delete st;

    return old_st;
}
