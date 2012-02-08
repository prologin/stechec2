#include "state.hh"

namespace rules {

RulesState::~RulesState()
{
    delete old_version_;
}

} // namespace rules
