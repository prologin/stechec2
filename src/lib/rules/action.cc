#include "action.hh"
#include "state.hh"

RulesState* Action::apply(RulesState* st) const
{
    // Copy the old state and link the new version to the old version.
    RulesState* new_st = st->copy();
    new_st->set_old_version(st);

    // Apply the action on the new state.
    this->apply_on(new_st);

    return new_st;
}
