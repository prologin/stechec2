// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <memory>
#include <string>

#include <zmq.hpp>

#include <utils/buffer.hh>

namespace net
{

struct Message;

// Socket is the common representation of the pair of ZeroMQ sockets Stechec
// needs to work. It is composed of the ZeroMQ context a PUB-SUB socket and
// a REQ-REP socket

class Socket // abstract
{
public:
    Socket(const std::string& pubsub_addr, const std::string& reqrep_addr,
           int io_thread);

    virtual ~Socket() = default;

    virtual void init()
    {
        shared_init();
    }
    virtual bool send(const utils::Buffer& msg, int flags = 0);
    virtual std::unique_ptr<utils::Buffer> recv(int flags = 0);
    virtual bool poll(long timeout);
    virtual void close() = 0;

protected:
    bool send_sckt(const utils::Buffer& buf, zmq::socket_t* sckt, int flags);
    std::unique_ptr<utils::Buffer> recv_sckt(zmq::socket_t* sckt, int flags);

protected:
    // Must be called by subclasses after custom initialization.
    void shared_init();

    std::string pubsub_addr_;
    std::string reqrep_addr_;

    zmq::context_t ctx_;
    std::unique_ptr<zmq::socket_t> pubsub_sckt_;
    std::unique_ptr<zmq::socket_t> reqrep_sckt_;
};

} // namespace net
