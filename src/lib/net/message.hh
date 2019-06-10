// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <cstdint>
#include <iostream>

#include <utils/buffer.hh>

namespace net {

// Message types of stechec2 (not the rules)
enum MsgType
{
    MSG_ERR = 0,
    MSG_CONNECT = 1,
    MSG_RULES = 2,
    MSG_IGNORED = 3,
    MSG_PLAYERS = 4,
    MSG_GAMESTART = 5,
    MSG_ACK = 6,
    MSG_EXIT = 7
};

struct Message
{
    explicit Message(uint32_t type_ = 0, uint32_t client_id_ = 0);

    void handle_buffer(utils::Buffer& buf)
    {
        buf.handle(type);
        buf.handle(client_id);
    }

    // String representation of a Message
    std::string str() const;

    // Type of the message (see MsgType)
    uint32_t type;

    // Id of the client that sent the message
    // Ignored for replies
    uint32_t client_id;
};

} // namespace net

std::ostream& operator<<(std::ostream& os, const net::Message& msg);
