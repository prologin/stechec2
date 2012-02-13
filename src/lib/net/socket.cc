#include "socket.hh"

#include <stdexcept>

#include <utils/log.hh>
#include <net/message.hh>

namespace net {

Socket::Socket(const std::string& pubsub_addr,
               const std::string& reqrep_addr,
               int io_thread)
    : pubsub_addr_(pubsub_addr),
      reqrep_addr_(reqrep_addr),
      ctx_(io_thread)
{
}

bool Socket::send_msg(const Message& msg)
{
    try
    {
        zmq::message_t zmsg(msg.size);
        memcpy(zmsg.data(), &msg, msg.size);

        if (!reqrep_sckt_->send(zmsg))
            throw std::runtime_error("Could not send message");

        return true;
    }
    catch(const std::exception& e)
    {
        ERR("%s", e.what());
        return false;
    }
}

bool Socket::get_msg(Message** msg)
{
    try
    {
        zmq::message_t zmsg;

        if (!reqrep_sckt_->recv(&zmsg))
            throw std::runtime_error("Could not get message");

        *msg = reinterpret_cast<Message*>(new char[zmsg.size()]);
        memcpy(*msg, zmsg.data(), zmsg.size());

        return true;
    }
    catch(const std::exception& e)
    {
        ERR("%s", e.what());
        return false;
    }
}

} // namespace net
