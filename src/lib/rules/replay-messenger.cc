#include "replay-messenger.hh"

#include "rules/actions.hh"

namespace rules
{

void ReplayMessenger::pull_id(uint32_t* player_id)
{
    buf_->handle(*player_id);
}

void ReplayMessenger::pull_actions(Actions* actions)
{
    buf_->handle_bufferizable(actions);
}

} // namespace rules
