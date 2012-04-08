#include "client.hh"

#include <utils/log.hh>
#include <net/common.hh>
#include <net/message.hh>
#include <net/client-messenger.hh>
#include <rules/player.hh>

#include "options.hh"

typedef void (*client_init)(net::ClientMessenger_sptr);
typedef bool (*client_turn)();
typedef void (*client_result)(rules::PlayerList*);

Client::Client(const Options& opt)
    : opt_(opt),
      rules_lib_(opt.rules_lib)
{
}

void Client::run()
{
    // Register to the stechec2 server
    sckt_init();

    // Get required functions from the rules library
    client_init rules_init = rules_lib_.get<client_init>("client_init");
    client_turn rules_turn = rules_lib_.get<client_turn>("client_turn");
    client_result rules_result = rules_lib_.get<client_result>("client_result");

    net::ClientMessenger_sptr client_msgr = net::ClientMessenger_sptr(
            new net::ClientMessenger(sckt_));

    rules_init(client_msgr);

    wait_for_game_start();

    while (rules_turn())
        ;

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
