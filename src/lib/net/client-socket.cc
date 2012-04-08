#include "client-socket.hh"

#include <zmq.hpp>

#include <utils/log.hh>

namespace net {

ClientSocket::ClientSocket(const std::string& sub_addr,
                           const std::string& req_addr)
    : Socket(sub_addr, req_addr, 1)
{
}

void ClientSocket::init()
{
    try
    {
        pubsub_sckt_ =
            std::shared_ptr<zmq::socket_t>(new zmq::socket_t(ctx_, ZMQ_SUB));
        pubsub_sckt_->connect(pubsub_addr_.c_str());
        pubsub_sckt_->setsockopt(ZMQ_SUBSCRIBE, nullptr, 0);
    }
    catch (const zmq::error_t& e)
    {
        FATAL("SUB: %s: %s", pubsub_addr_.c_str(), e.what());
    }

    try
    {
        reqrep_sckt_ =
            std::shared_ptr<zmq::socket_t>(new zmq::socket_t(ctx_, ZMQ_REQ));
        reqrep_sckt_->connect(reqrep_addr_.c_str());
    }
    catch (const zmq::error_t& e)
    {
        FATAL("REQ: %s: %s", reqrep_addr_.c_str(), e.what());
    }
}

Message* ClientSocket::pull(int flags)
{
    return recv_sckt(pubsub_sckt_, flags);
}

} // namespace net