#include "client.hh"

#include <utils/log.hh>
#include <net/common.hh>
#include <net/message.hh>
#include <rules/action.hh>
#include <rules/player.hh>

#include "options.hh"

Client::Client(const Options& opt)
    : opt_(opt)
{
    // Get required functions from the rules library
    utils::DLL rules_lib(opt.rules_lib);
    rules_init = rules_lib.get<rules::f_rules_init>("rules_init");
    client_loop = rules_lib.get<rules::f_client_loop>("client_loop");
    rules_result = rules_lib.get<rules::f_rules_result>("rules_result");
}

void Client::run()
{
    // Register to the stechec2 server
    sckt_init();

    // Create a messenger for sending rules messages
    msgr_ = net::ClientMessenger_sptr(new net::ClientMessenger(sckt_));

    // Rules specific initializations
    rules_init();

    // Wait for the server ACK to start the game
    wait_for_game_start();

    client_loop(msgr_);

    rules::PlayerList players;
    rules_result(&players);
}

void Client::sckt_init()
{
    sckt_ = net::ClientSocket_sptr(
            new net::ClientSocket(opt_.sub_addr, opt_.req_addr));
    sckt_->init();

    NOTICE("Requesting on %s", opt_.req_addr.c_str());
    NOTICE("Subscribing on %s", opt_.sub_addr.c_str());

    // Send a message to get an ID from the server
    // To avoid useless message, the client_id of the request corresponds
    // to the type of the client connecting (PLAYER, SPECTATOR, ...)
    net::Message id_req(net::MSG_CONNECT, net::PLAYER);
    net::Message* id_rep = nullptr;

    if (!sckt_->send(id_req) || !(id_rep = sckt_->recv()) ||
            id_rep->client_id == 0)
        FATAL("Unable to get an ID from the server");

    id_ = id_rep->client_id;

    delete id_rep;

    NOTICE("Connected - id: %i", id_);
}

void Client::wait_for_game_start()
{
    net::Message* msg = nullptr;
    uint32_t msg_type = net::MSG_IGNORED;

    while (msg_type != net::MSG_GAMESTART)
    {
        msg = sckt_->pull();
        msg_type = msg->type;
        delete msg;
    }
}
