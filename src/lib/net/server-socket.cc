// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "server-socket.hh"

#include <memory>
#include <sys/stat.h>
#include <sys/types.h>
#include <zmq.hpp>

#include <chrono>
#include <thread>

#include <utils/log.hh>

namespace net {

ServerSocket::ServerSocket(const std::string& pub_addr,
                           const std::string& rep_addr)
    : Socket(pub_addr, rep_addr, 2)
{}

void ServerSocket::init()
{
    // We want to create the domain sockets with mode 777 to allow users from
    // different processes to communicate with us.
    mode_t old_umask = umask(0);

    try
    {
        pubsub_sckt_ = std::make_unique<zmq::socket_t>(ctx_, ZMQ_PUB);
        pubsub_sckt_->bind(pubsub_addr_.c_str());
    }
    catch (const zmq::error_t& e)
    {
        FATAL("PUB: %s: %s", pubsub_addr_.c_str(), e.what());
    }

    try
    {
        reqrep_sckt_ = std::make_unique<zmq::socket_t>(ctx_, ZMQ_REP);
        reqrep_sckt_->bind(reqrep_addr_.c_str());
    }
    catch (const zmq::error_t& e)
    {
        FATAL("REP: %s: %s", reqrep_addr_.c_str(), e.what());
    }

    // Restore the process umask
    umask(old_umask);

    shared_init();
}

void ServerSocket::close()
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

bool ServerSocket::push(const utils::Buffer& buf, int flags, size_t sleep)
{
    if (sleep)
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
    return send_sckt(buf, pubsub_sckt_.get(), flags);
}

} // namespace net
