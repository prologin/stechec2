#include "client.hh"

#include <utils/log.hh>
#include <net/client.hh>
#include <net/message.hh>

#include "options.hh"

Client::Client(const Options& opt)
    : opt_(opt)
{
}

void Client::run()
{
    init();
}

void Client::init()
{
    net_ = std::unique_ptr<net::Client>(
            new net::Client(opt_.sub_addr, opt_.req_addr));
    net_->init();

    NOTICE("Requesting on %s", opt_.req_addr.c_str());
    NOTICE("Subscribing on %s", opt_.sub_addr.c_str());

    // Send a message to get an ID from the server
    net::Message id_req(net::MSG_GETID);
    net::Message* id_rep = nullptr;

    if (!net_->send_msg(id_req) || !(id_rep = net_->get_msg()) ||
            id_rep->client_id == 0)
        FATAL("%s", "Unable to get an ID from the server");

    id_ = id_rep->client_id;

    delete id_rep;

    NOTICE("Connected with id %i", id_);
}
