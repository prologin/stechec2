#include "socket.hh"

#include <stdexcept>

#include <utils/log.hh>
#include <network/message.hh>

namespace network {

Socket::Socket(const std::string& pubsub_addr,
               const std::string& reqrep_addr,
               int io_thread)
    : pubsub_addr_(pubsub_addr),
      reqrep_addr_(reqrep_addr),
      ctx_(io_thread)
{
}

Socket::~Socket()
{
    delete pubsub_sckt_;
    delete reqrep_sckt_;
}

void Socket::send_msg(const Message& msg)
{
    try
    {
        zmq::message_t zmsg(msg.size);
        memcpy(zmsg.data(), &msg, msg.size);

        if (!reqrep_sckt_->send(zmsg))
            throw std::runtime_error("Could not send message");
    }
    catch(const std::exception& e)
    {
        FATAL("%s", e.what());
    }
}

void Socket::get_msg(Message* msg)
{
    try
    {
        zmq::message_t zmsg;

        if (!reqrep_sckt_->recv(&zmsg))
            throw std::runtime_error("Could not get message");

        memcpy(msg, zmsg.data(), zmsg.size());
    }
    catch(const std::exception& e)
    {
        FATAL("%s", e.what());
    }
}

} // namespace network
