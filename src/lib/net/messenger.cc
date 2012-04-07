#include "messenger.hh"

namespace net {

Messenger::~Messenger()
{
}

Message* Messenger::to_msg(const void* data, uint32_t data_size)
{
    Message* msg = reinterpret_cast<Message*>(
            new char[sizeof (Message) + data_size]);

    msg->type = MSG_RULES;
    msg->client_id = 0;
    msg->size = data_size;

    memcpy(msg->data, data, data_size);

    return msg;
}

uint32_t Messenger::from_msg(const Message& msg, void** data)
{
    *data = reinterpret_cast<void*>(new char[msg.size]);

    memcpy(*data, msg.data, msg.size);

    return msg.size;
}

} // namespace net
