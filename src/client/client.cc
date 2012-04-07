#include "client.hh"

#include <utils/log.hh>
#include <net/common.hh>
#include <net/message.hh>
#include <net/client-messenger.hh>

#include "options.hh"

typedef void (*client_loop)(net::ClientMessenger_sptr);

Client::Client(const Options& opt)
    : opt_(opt),
      rules_lib_(opt.rules_lib)
{
}

void Client::run()
{
    init();

    // Get the game loop function from the rules library
    client_loop game_loop = rules_lib_.get<client_loop>("client_loop");
    net::ClientMessenger_sptr client_msgr = net::ClientMessenger_sptr(
            new net::ClientMessenger(net_));

    game_loop(client_msgr);
}

void Client::init()
{
    net_ = net::ClientSocket_sptr(
            new net::ClientSocket(opt_.sub_addr, opt_.req_addr));
    net_->init();

    NOTICE("Requesting on %s", opt_.req_addr.c_str());
    NOTICE("Subscribing on %s", opt_.sub_addr.c_str());

    // Send a message to get an ID from the server
    // To avoid useless message, the client_id of the request corresponds
    // to the type of the client connecting (PLAYER, SPECTATOR, ...)
    net::Message id_req(net::MSG_CONNECT, net::PLAYER);
    net::Message* id_rep = nullptr;

    if (!net_->send(id_req) || !(id_rep = net_->recv()) ||
            id_rep->client_id == 0)
        FATAL("Unable to get an ID from the server");

    id_ = id_rep->client_id;

    delete id_rep;

    NOTICE("Connected - id: %i", id_);
}
