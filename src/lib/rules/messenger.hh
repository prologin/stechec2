// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <cstdint>
#include <net/message.hh>
#include <net/socket.hh>
#include <utils/buffer.hh>

namespace rules {

class Messenger
{
public:
    virtual ~Messenger();

    virtual void send(const utils::Buffer&) = 0;
    virtual utils::Buffer* recv() = 0;
};

} // namespace rules
