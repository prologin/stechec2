#include "server-messenger.hh"

#include <net/message.hh>
#include <rules/actions.hh>

namespace rules {

ServerMessenger::ServerMessenger(net::ServerSocket_sptr sckt)
    : sckt_(std::move(sckt))
    , last_client_id_(-1)
{
}

void ServerMessenger::send(const utils::Buffer& buf)
{
    utils::Buffer out_buf;
    net::Message msg(net::MSG_RULES);

    msg.handle_buffer(out_buf);
    out_buf += buf;

    if (!sckt_->send(out_buf))
        FATAL("Unable to send data to client");
}

void ServerMessenger::push(const utils::Buffer& buf)
{
    utils::Buffer out_buf;
    net::Message msg(net::MSG_RULES);

    msg.handle_buffer(out_buf);
    out_buf += buf;

    sckt_->push(out_buf);
}

bool ServerMessenger::poll(long timeout)
{
    return sckt_->poll(timeout);
}

void ServerMessenger::push_actions(Actions& actions)
{
    utils::Buffer buf;
    actions.handle_buffer(buf);
    push(buf);
}

void ServerMessenger::push_id(uint32_t id)
{
    utils::Buffer buf;
    buf.handle(id);
    push(buf);
}

utils::Buffer* ServerMessenger::recv()
{
    utils::Buffer* buf = sckt_->recv();
    if (!buf)
        FATAL("Unable to receive data from client");

    net::Message msg;
    msg.handle_buffer(*buf);

    last_client_id_ = msg.client_id;
    CHECK_EXC(ServerMessengerError, msg.type == net::MSG_RULES);

    return buf;
}

void ServerMessenger::recv_actions(Actions* actions)
{
    utils::Buffer* buf = recv();

    actions->handle_buffer(*buf);

    delete buf;
}

void ServerMessenger::ack()
{
    utils::Buffer buf;
    net::Message msg(net::MSG_ACK);

    msg.handle_buffer(buf);

    if (!sckt_->send(buf))
        FATAL("Unable to send ack to client");
}

void ServerMessenger::wait_for_ack()
{
    utils::Buffer* buf = sckt_->recv();
    if (!buf)
        FATAL("Unable to receive ack from client");

    net::Message msg;
    msg.handle_buffer(*buf);

    CHECK_EXC(ServerMessengerError, msg.type == net::MSG_ACK);
}

} // namespace net
