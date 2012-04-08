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

bool Socket::send(const Message& msg, int flags)
{
    return send_sckt(msg, reqrep_sckt_, flags);
}

Message* Socket::recv(int flags)
{
    return recv_sckt(reqrep_sckt_, flags);
}

bool Socket::send_sckt(const Message& msg, std::shared_ptr<zmq::socket_t> sckt,
        int flags)
{
    try
    {
        zmq::message_t zmsg(sizeof (Message) + msg.size);
        memcpy(zmsg.data(), &msg, sizeof (Message) + msg.size);

        if (!sckt->send(zmsg, flags))
            throw std::runtime_error("Could not send message");

        return true;
    }
    catch(const std::exception& e)
    {
        ERR("%s", e.what());
        return false;
    }
}

Message* Socket::recv_sckt(std::shared_ptr<zmq::socket_t> sckt, int flags)
{
    try
    {
        zmq::message_t zmsg;

        if (!sckt->recv(&zmsg, flags))
            throw std::runtime_error("Could not get message");

        Message* msg = reinterpret_cast<Message*>(new char[zmsg.size()]);
        memcpy(msg, zmsg.data(), zmsg.size());

        return msg;
    }
    catch(const std::exception& e)
    {
        ERR("%s", e.what());
        return nullptr;
    }
}

} // namespace net
