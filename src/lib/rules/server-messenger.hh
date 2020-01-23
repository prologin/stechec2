// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <memory>
#include <stdexcept>

#include <net/server-socket.hh>
#include <rules/messenger.hh>
#include <utils/buffer.hh>

namespace rules {

class Actions;

class ServerMessengerError : public std::runtime_error
{
public:
    ServerMessengerError() : std::runtime_error("Server messenger error") {}
};

class ServerMessenger : public Messenger
{
public:
    explicit ServerMessenger(net::ServerSocket* sckt);

    void send(const utils::Buffer&) override;

    std::unique_ptr<utils::Buffer> recv() override;
    void recv_actions(Actions* actions);

    bool poll(long timeout);

    void push(const utils::Buffer&);
    void push_actions(Actions& actions);
    void push_id(uint32_t id);

    void ack();
    void wait_for_ack();

    uint32_t last_client_id() { return last_client_id_; }

private:
    net::ServerSocket* sckt_; // Not owned.
    uint32_t last_client_id_;
};

using ServerMessenger_sptr = std::shared_ptr<ServerMessenger>;

} // namespace rules
