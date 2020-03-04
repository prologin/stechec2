// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#pragma once

#include <rules/messenger.hh>

namespace rules
{

class Actions;

class ReplayMessenger
{
public:
    explicit ReplayMessenger(utils::Buffer* buf)
        : buf_(buf)
    {
    }

    void pull_id(uint32_t* player_id);
    void pull_actions(Actions* actions);

private:
    utils::Buffer* buf_;
};

using ReplayMessenger_sptr = std::shared_ptr<ReplayMessenger>;

} // namespace rules
