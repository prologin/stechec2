#include "client.hh"

#include <utils/log.hh>
#include <net/client.hh>
#include <net/message.hh>

void Client::run()
{
    init();
}

void Client::init()
{
    net_ = new net::Client("tcp://127.0.0.1:2345",
                               "tcp://127.0.0.1:2346");

    net_->init();

    // Send a message to get an ID from the server
    net::Message id_req(net::MSG_GETID);
    net::Message* id_rep = nullptr;

    if (!net_->send_msg(id_req) || !net_->get_msg(&id_rep) ||
            id_rep->client_id == 0)
        FATAL("%s", "Unable to get an ID from the server");

    id_ = id_rep->client_id;

    delete id_rep;

    NOTICE("Connected with id %i", id_);
}
