// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <memory>
#include <stdexcept>

#include <net/client-socket.hh>
#include <rules/messenger.hh>
#include <utils/buffer.hh>

namespace rules {

// Forward decls
class Actions;

class ClientMessengerError : public std::runtime_error
{
public:
    ClientMessengerError() : std::runtime_error("Client messenger error") {}
};

class ClientMessenger : public Messenger
{
public:
    ClientMessenger(net::ClientSocket* sckt, uint32_t client_id);

    void send(const utils::Buffer&) override;
    void send_actions(Actions&);

    std::unique_ptr<utils::Buffer> recv() override;

    std::unique_ptr<utils::Buffer> pull();
    void pull_actions(Actions*);
    void pull_id(uint32_t*);

    void ack();
    void wait_for_ack();

    /* Wait until a new turn. Put the ID of the corresponding player into
     * `pulled_id` and return if this is the same as `player_id`. */
    bool wait_for_turn(uint32_t player_id, uint32_t* pulled_id);

private:
    net::ClientSocket* sckt_; // Not owned.
    uint32_t client_id_;
};

using ClientMessenger_sptr = std::shared_ptr<ClientMessenger>;

} // namespace rules
