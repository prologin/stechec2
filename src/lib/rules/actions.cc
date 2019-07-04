// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include <utility>

#include "actions.hh"

namespace rules {

void Actions::handle_buffer(utils::Buffer& buf)
{
    if (buf.serialize())
    {
        for (const auto& action : actions_)
        {
            // The id is needed to reconstruct the action when unserializing
            uint32_t action_id = action->id();
            buf.handle(action_id);

            action->handle_buffer(buf);
        }
    }
    else
    {
        while (!buf.empty())
        {
            // Get the action id
            uint32_t action_id;
            buf.handle(action_id);

            // Use it to instantiate the right rules Action
            if (action_factory_.find(action_id) == action_factory_.end())
                FATAL("Action %d isn't registered", action_id);
            auto action = action_factory_[action_id]();

            // And finally unserialize
            action->handle_buffer(buf);
            actions_.push_back(std::move(action));
        }
    }
}

void Actions::register_action(uint32_t action_id, ActionFactory action_factory)
{
    action_factory_[action_id] = std::move(action_factory);
}

} // namespace rules
