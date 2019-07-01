// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "client-socket.hh"

#include <zmq.hpp>

#include <utils/log.hh>

namespace net {

ClientSocket::ClientSocket(const std::string& sub_addr,
                           const std::string& req_addr)
    : Socket(sub_addr, req_addr, 1)
{}

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

    shared_init();
}

void ClientSocket::close()
{
    try
    {
        pubsub_sckt_->close();
    }
    catch (const zmq::error_t& e)
    {
        FATAL("SUB: close: %s", e.what());
    }

    try
    {
        reqrep_sckt_->close();
    }
    catch (const zmq::error_t& e)
    {
        FATAL("REQ: close: %s", e.what());
    }
}

std::unique_ptr<utils::Buffer> ClientSocket::pull(int flags)
{
    return recv_sckt(pubsub_sckt_, flags);
}

} // namespace net
