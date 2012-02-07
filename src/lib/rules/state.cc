#include "state.hh"

RulesState::~RulesState()
{
    delete old_version_;
}
