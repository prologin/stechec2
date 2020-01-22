// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <cstdint>
#include <net/message.hh>
#include <net/socket.hh>
#include <utils/buffer.hh>

namespace rules {

class Messenger
{
public:
    virtual ~Messenger() = default;

    virtual void send(const utils::Buffer&) = 0;
    virtual std::unique_ptr<utils::Buffer> recv() = 0;
};

} // namespace rules
