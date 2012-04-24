#include "server-messenger.hh"

#include <net/message.hh>

namespace rules {

ServerMessenger::ServerMessenger(net::ServerSocket_sptr sckt)
    : sckt_(sckt)
{
}

void ServerMessenger::send(const utils::Buffer& buf)
{
    utils::Buffer out_buf;
    net::Message msg(net::MSG_RULES);

    msg.handle_buffer(out_buf);
    out_buf += buf;

    sckt_->send(out_buf);
}

void ServerMessenger::push(const utils::Buffer& buf)
{
    utils::Buffer out_buf;
    net::Message msg(net::MSG_RULES);

    msg.handle_buffer(out_buf);
    out_buf += buf;

    sckt_->push(out_buf);
}

utils::Buffer* ServerMessenger::recv()
{
    utils::Buffer* buf = sckt_->recv();

    net::Message msg;
    msg.handle_buffer(*buf);

    return buf;
}

void ServerMessenger::ack()
{
    utils::Buffer buf;
    net::Message msg(net::MSG_ACK);

    msg.handle_buffer(buf);

    sckt_->send(buf);
}

} // namespace net
