#include "client-messenger.hh"

#include <utils/buffer.hh>

namespace net {

ClientMessenger::ClientMessenger(ClientSocket_sptr sckt)
    : sckt_(sckt)
{
}

void ClientMessenger::send(RulesMessage* rules_msg)
{
    utils::Buffer buf;

    rules_msg->handle_buffer(&buf);
    Message* msg = to_msg(buf.data(), buf.length());

    sckt_->send(*msg);
}

void ClientMessenger::recv(RulesMessage* rules_msg)
{
    Message* msg = sckt_->recv();

    uint8_t* data;
    uint32_t size = from_msg(*msg, &data);

    std::vector<uint8_t> data_vector;
    data_vector.assign(data, data + size);

    utils::Buffer buf(data_vector);

    rules_msg->handle_buffer(&buf);
}

} // namespace net
