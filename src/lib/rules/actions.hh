// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <cstdint>
#include <unordered_map>

#include <utils/buffer.hh>
#include <utils/log.hh>

#include "action.hh"

namespace rules {

class Actions : public utils::IBufferizable
{
    using ActionFactory = std::function<std::unique_ptr<IAction>()>;
    using ActionLog = std::vector<std::unique_ptr<IAction>>;

public:
    static constexpr size_t MAX_ACTIONS = 1024;

    // To handle unserialization of multiple Actions, we have to be able to
    // instantiate the corresponding objects
    void register_action(uint32_t action_id, ActionFactory action_factory);

    // Serialization/Unserialization
    void handle_buffer(utils::Buffer& buf) override;

    void add(std::unique_ptr<IAction> action)
    {
        if (actions_.size() >= MAX_ACTIONS)
            FATAL("Too many actions (>%d) sent during this turn.", MAX_ACTIONS);
        actions_.push_back(std::move(action));
    }

    void cancel() { actions_.pop_back(); }

    size_t size() const { return actions_.size(); }

    void clear() { actions_.clear(); }

    const ActionLog& all() const { return actions_; }
    // Deprecated: the action buffer should not be mutable.
    ActionLog& all() { return actions_; }

private:
    ActionLog actions_;
    std::unordered_map<uint32_t, ActionFactory> action_factory_;
};

} // namespace rules
