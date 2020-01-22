// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <memory>
#include <string>

#include <net/message.hh>
#include <net/socket.hh>
#include <utils/buffer.hh>

namespace net {

// Because it inherits Socket, ServerSocket's field names are pubsub* and
// reqrep* to be consistent with ClientSocket

class ServerSocket : public Socket
{
public:
    ServerSocket(const std::string& pub_addr, const std::string& rep_addr);

    void init() override;
    void close() override;

    bool push(const utils::Buffer& buf,
              zmq::send_flags flags = zmq::send_flags::none, size_t sleep = 0);
};

using ServerSocket_sptr = std::shared_ptr<ServerSocket>;

} // namespace net
