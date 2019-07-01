// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "client-messenger.hh"

#include <net/message.hh>
#include <rules/actions.hh>
#include <utils/log.hh>

namespace rules {

ClientMessenger::ClientMessenger(net::ClientSocket_sptr sckt,
                                 uint32_t client_id)
    : sckt_(std::move(sckt)), client_id_(client_id)
{}

void ClientMessenger::send(const utils::Buffer& buf)
{
    utils::Buffer out_buf;
    net::Message msg(net::MSG_RULES, client_id_);

    msg.handle_buffer(out_buf);
    out_buf += buf;

    if (!sckt_->send(out_buf))
        FATAL("Unable to send message to server");
}

void ClientMessenger::send_actions(Actions& actions)
{
    utils::Buffer buf;
    actions.handle_buffer(buf);
    send(buf);
}

std::unique_ptr<utils::Buffer> ClientMessenger::recv()
{
    auto buf = sckt_->recv();
    if (!buf)
        FATAL("Unable to receive message from server");

    net::Message msg;
    msg.handle_buffer(*buf);

    return buf;
}

std::unique_ptr<utils::Buffer> ClientMessenger::pull()
{
    auto buf = sckt_->pull();

    net::Message msg;
    msg.handle_buffer(*buf);

    return buf;
}

void ClientMessenger::pull_actions(Actions* actions)
{
    actions->handle_buffer(*pull());
}

void ClientMessenger::pull_id(uint32_t* id)
{
    pull()->handle(*id);
}

void ClientMessenger::ack()
{
    utils::Buffer buf;
    net::Message msg(net::MSG_ACK);

    msg.handle_buffer(buf);

    if (!sckt_->send(buf))
        FATAL("Unable to send ack to server");
}

void ClientMessenger::wait_for_ack()
{
    auto buf = sckt_->recv();
    if (!buf)
        FATAL("Unable to receive ack from server");

    net::Message msg;
    msg.handle_buffer(*buf);

    CHECK_EXC(ClientMessengerError, msg.type == net::MSG_ACK);
}

bool ClientMessenger::wait_for_turn(uint32_t player_id, uint32_t* pulled_id)
{
    pull_id(pulled_id);

    return player_id != *pulled_id;
}

} // namespace rules
