#include "server.hh"

#include <memory>
#include <zmq.hpp>

#include <utils/log.hh>

namespace net {

Server::Server(const std::string& pub_addr, const std::string& rep_addr)
    : Socket(pub_addr, rep_addr, 2)
{
}

void Server::init()
{
    try
    {
        pubsub_sckt_ =
            std::unique_ptr<zmq::socket_t>(new zmq::socket_t(ctx_, ZMQ_PUB));
        pubsub_sckt_->bind(pubsub_addr_.c_str());
    }
    catch (const zmq::error_t& e)
    {
        FATAL("PUB: %s: %s", pubsub_addr_.c_str(), e.what());
    }

    try
    {
        reqrep_sckt_ =
            std::unique_ptr<zmq::socket_t>(new zmq::socket_t(ctx_, ZMQ_REP));
        reqrep_sckt_->bind(reqrep_addr_.c_str());
    }
    catch (const zmq::error_t& e)
    {
        FATAL("REP: %s: %s", reqrep_addr_.c_str(), e.what());
    }
}

} // namespace net
