#include "client-messenger.hh"

namespace net {

ClientMessenger::ClientMessenger(ClientSocket_sptr sckt)
    : sckt_(sckt)
{
}

void ClientMessenger::send(const utils::Buffer& buf)
{
    Message* msg = to_msg(buf.data(), buf.size());
    sckt_->send(*msg);
    delete msg;
}

utils::Buffer* ClientMessenger::recv()
{
    Message* msg = sckt_->recv();
    utils::Buffer* buf = internal_recv(msg);
    delete msg;

    return buf;
}

utils::Buffer* ClientMessenger::pull()
{
    Message* msg = sckt_->pull();
    utils::Buffer* buf = internal_recv(msg);
    delete msg;

    return buf;
}

utils::Buffer* ClientMessenger::internal_recv(Message* msg)
{
    uint8_t* data;
    uint32_t size = from_msg(*msg, &data);

    std::vector<uint8_t> data_vector;
    data_vector.assign(data, data + size);

    utils::Buffer* buf = new utils::Buffer(data_vector);

    return buf;
}

} // namespace net
