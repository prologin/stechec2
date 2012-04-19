#include "messenger.hh"

namespace net {

Messenger::~Messenger()
{
}

Message* Messenger::to_msg(const uint8_t* data, uint32_t data_size)
{
    Message* msg = reinterpret_cast<Message*>(
            new uint8_t[sizeof (Message) + data_size]);

    msg->type = MSG_RULES;
    msg->client_id = 0;
    msg->size = data_size;

    memcpy(msg->data, data, data_size);

    return msg;
}

uint32_t Messenger::from_msg(const Message& msg, uint8_t** data)
{
    *data = new uint8_t[msg.size];

    memcpy(*data, msg.data, msg.size);

    return msg.size;
}

} // namespace net
