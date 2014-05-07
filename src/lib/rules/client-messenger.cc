#include "client-messenger.hh"

#include <utils/log.hh>
#include <net/message.hh>
#include <rules/actions.hh>

namespace rules {

ClientMessenger::ClientMessenger(net::ClientSocket_sptr sckt)
    : sckt_(sckt)
{
}

void ClientMessenger::send(const utils::Buffer& buf, uint32_t id)
{
    utils::Buffer out_buf;
    net::Message msg(net::MSG_RULES, id);

    msg.handle_buffer(out_buf);
    out_buf += buf;

    sckt_->send(out_buf);
}

void ClientMessenger::send_actions(Actions& actions, uint32_t id)
{
    utils::Buffer buf;
    actions.handle_buffer(buf);
    send(buf, id);
}

utils::Buffer* ClientMessenger::recv()
{
    utils::Buffer* buf = sckt_->recv();

    net::Message msg;
    msg.handle_buffer(*buf);

    return buf;
}

utils::Buffer* ClientMessenger::pull()
{
    utils::Buffer* buf = sckt_->pull();

    net::Message msg;
    msg.handle_buffer(*buf);

    return buf;
}

void ClientMessenger::pull_actions(Actions* actions)
{
    utils::Buffer* buf = pull();
    actions->handle_buffer(*buf);
    delete buf;
}

void ClientMessenger::pull_id(uint32_t* id)
{
    utils::Buffer* buf = pull();
    buf->handle(*id);
    delete buf;
}

void ClientMessenger::ack()
{
    utils::Buffer buf;
    net::Message msg(net::MSG_ACK);

    msg.handle_buffer(buf);

    sckt_->send(buf);
}

void ClientMessenger::wait_for_ack()
{
    utils::Buffer* buf = sckt_->recv();

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
