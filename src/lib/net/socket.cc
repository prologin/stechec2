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

bool Socket::send(const utils::Buffer& msg, int flags)
{
    return send_sckt(msg, reqrep_sckt_, flags);
}

utils::Buffer* Socket::recv(int flags)
{
    return recv_sckt(reqrep_sckt_, flags);
}

bool Socket::send_sckt(const utils::Buffer& buf,
        std::shared_ptr<zmq::socket_t> sckt, int flags)
{
    try
    {
        zmq::message_t zmsg(buf.size());
        memcpy(zmsg.data(), buf.data(), buf.size());

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

utils::Buffer* Socket::recv_sckt(std::shared_ptr<zmq::socket_t> sckt, int flags)
{
    try
    {
        zmq::message_t zmsg;

        if (!sckt->recv(&zmsg, flags))
            throw std::runtime_error("Could not get message");

        std::vector<uint8_t> data;
        data.assign((uint8_t*)zmsg.data(), (uint8_t*)zmsg.data() + zmsg.size());

        utils::Buffer* buf = new utils::Buffer(data);

        return buf;
    }
    catch(const std::exception& e)
    {
        ERR("%s", e.what());
        return nullptr;
    }
}

} // namespace net
