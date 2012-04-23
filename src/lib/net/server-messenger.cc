#include "server-messenger.hh"

namespace net {

ServerMessenger::ServerMessenger(ServerSocket_sptr sckt)
    : sckt_(sckt)
{
}

void ServerMessenger::send(const utils::Buffer& buf)
{
    utils::Buffer out_buf;
    Message msg(MSG_RULES);

    msg.handle_buffer(out_buf);
    out_buf += buf;

    sckt_->send(out_buf);
}

void ServerMessenger::push(const utils::Buffer& buf)
{
    utils::Buffer out_buf;
    Message msg(MSG_RULES);

    msg.handle_buffer(out_buf);
    out_buf += buf;

    sckt_->push(out_buf);
}

utils::Buffer* ServerMessenger::recv()
{
    utils::Buffer* buf = sckt_->recv();

    Message msg;
    msg.handle_buffer(*buf);

    return buf;
}

void ServerMessenger::ack()
{
    utils::Buffer buf;
    Message msg(MSG_ACK);

    msg.handle_buffer(buf);

    sckt_->send(buf);
}

} // namespace net
