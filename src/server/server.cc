#include "server.hh"

#include <utils/log.hh>
#include <net/message.hh>
#include <net/common.hh>
#include <net/server-messenger.hh>
#include <rules/player.hh>

#include "options.hh"

typedef void (*server_init)(net::ServerMessenger_sptr);
typedef bool (*server_turn)();
typedef void (*server_result)(rules::PlayerList*);

Server::Server(const Options& opt)
    : opt_(opt),
      rules_lib_(opt.rules_lib),
      nb_clients_(0)
{
}

void Server::run()
{
    // Launch the network server, listen for connections
    net_init();

    INFO("Server Started");

    // We have to wait for the required number of clients specified in the
    // config to be met
    wait_for_players();

    // Get required functions from the rules library
    server_init rules_init = rules_lib_.get<server_init>("server_init");
    server_turn rules_turn = rules_lib_.get<server_turn>("server_turn");
    server_result rules_result = rules_lib_.get<server_result>("server_result");

    net::ServerMessenger_sptr server_msgr = net::ServerMessenger_sptr(
            new net::ServerMessenger(net_));

    rules_init(server_msgr);

    while (rules_turn())
        ;

    rules::PlayerList players;
    rules_result(&players);
}

void Server::net_init()
{
    net_ = net::ServerSocket_sptr(
            new net::ServerSocket(opt_.pub_addr, opt_.rep_addr));
    net_->init();

    NOTICE("Replying on %s", opt_.rep_addr.c_str());
    NOTICE("Publishing on %s", opt_.pub_addr.c_str());
}

void Server::wait_for_players()
{
    // For each client connecting, we send back a unique id
    // Clients are players or spectators

    while (clients_.size() < opt_.nb_clients)
    {
        net::Message* id_req = nullptr;

        if (!(id_req = net_->recv()))
            continue;

        if (id_req->type != net::MSG_CONNECT)
        {
            ERR("Message is not of type MSG_CONNECT, ignoring request");
            continue;
        }

        // To avoid useless message, the client_id of the request corresponds
        // to the type of the client connecting (PLAYER, SPECTATOR, ...)
        Client_ptr new_client = Client_ptr(new Client(++nb_clients_,
                        (net::ClientType) id_req->client_id));

        net::Message id_rep(net::MSG_CONNECT, new_client->id());
        net_->send(id_rep);

        clients_.push_back(new_client);

        NOTICE("Client connected - id: %i - type: %s", new_client->id(),
                clienttype_str(new_client->type()).c_str());

        delete id_req;
    }
}
