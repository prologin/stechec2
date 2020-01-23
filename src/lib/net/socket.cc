// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "socket.hh"

#include <stdexcept>

#include <net/message.hh>
#include <utils/log.hh>

DEFINE_int32(socket_timeout, -1,
             "Timeout value to use for all sock ops, in ms.");

namespace net {

Socket::Socket(const std::string& pubsub_addr, const std::string& reqrep_addr,
               int io_thread)
    : pubsub_addr_(pubsub_addr)
    , reqrep_addr_(reqrep_addr)
    , ctx_(io_thread)
    , pubsub_sckt_()
    , reqrep_sckt_()
{}

void Socket::shared_init()
{
    int timeout = FLAGS_socket_timeout;
    if (timeout != -1)
    {
        pubsub_sckt_->setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
        reqrep_sckt_->setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    }
}

bool Socket::send(const utils::Buffer& msg, int flags)
{
    return send_sckt(msg, reqrep_sckt_.get(), flags);
}

std::unique_ptr<utils::Buffer> Socket::recv(int flags)
{
    return recv_sckt(reqrep_sckt_.get(), flags);
}

bool Socket::poll(long timeout)
{
    zmq::pollitem_t pollitem;
    pollitem.socket = static_cast<void*>(reqrep_sckt_.get());
    pollitem.events = ZMQ_POLLIN;

    return zmq::poll(&pollitem, 1, timeout) > 0;
}

bool Socket::send_sckt(const utils::Buffer& buf, zmq::socket_t* sckt, int flags)
{
    try
    {
        while (true)
            try
            {
                if (!sckt->send(buf.data(), buf.size(), flags))
                    throw std::runtime_error("Could not send message");
                break;
            }
            catch (const zmq::error_t& e)
            {
                if (e.num() == EINTR)
                    continue;
                throw;
            }
        return true;
    }
    catch (const std::exception& e)
    {
        ERR("%s", e.what());
        return false;
    }
}

std::unique_ptr<utils::Buffer>
Socket::recv_sckt(zmq::socket_t* sckt, int flags)
{
    try
    {
        zmq::message_t zmsg;
        while (true)
            try
            {
                if (!sckt->recv(&zmsg, flags))
                    throw std::runtime_error("Could not get message");
                break;
            }
            catch (const zmq::error_t& e)
            {
                if (e.num() == EINTR)
                    continue;
                throw;
            }

        std::vector<uint8_t> data;
        data.assign(reinterpret_cast<uint8_t*>(zmsg.data()),
                    reinterpret_cast<uint8_t*>(zmsg.data()) + zmsg.size());

        return std::make_unique<utils::Buffer>(data);
    }
    catch (const std::exception& e)
    {
        ERR("%s", e.what());
        return nullptr;
    }
}

} // namespace net
