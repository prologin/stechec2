// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <memory>
#include <string>

#include <net/socket.hh>
#include <utils/buffer.hh>

namespace net {

// Because it inherits Socket, ClientSocket's field names are pubsub* and
// reqrep* to be consistent with ServerSocket

class ClientSocket : public Socket
{
public:
    ClientSocket(const std::string& sub_addr, const std::string& req_addr);

    void init() override;
    void close() override;

    utils::Buffer* pull(int flags = 0);
};

typedef std::shared_ptr<ClientSocket> ClientSocket_sptr;

} // namespace net
