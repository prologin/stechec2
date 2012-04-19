#include "server.hh"

#include <utils/log.hh>
#include <net/message.hh>
#include <net/common.hh>
#include <rules/player.hh>

#include "options.hh"

Server::Server(const Options& opt)
    : opt_(opt),
      nb_clients_(0)
{
    // Get required functions from the rules library
    utils::DLL rules_lib(opt.rules_lib);
    rules_init = rules_lib.get<rules::f_rules_init>("rules_init");
    server_loop = rules_lib.get<rules::f_server_loop>("server_loop");
    rules_result = rules_lib.get<rules::f_rules_result>("rules_result");
}

void Server::run()
{
    // Launch the network server, listen for connections
    sckt_init();

    INFO("Server Started");

    // We have to wait for the required number of clients specified in the
    // config to be met
    wait_for_players();

    // Create a messenger for sending rules messages
    msgr_ = net::ServerMessenger_sptr(new net::ServerMessenger(sckt_));

    // Rules specific initializations
    rules_init();

    // Send the server ACK to start the game
    sckt_->push(net::Message(net::MSG_GAMESTART));

    server_loop(msgr_);

    rules::PlayerList players;
    rules_result(&players);
}

void Server::sckt_init()
{
    sckt_ = net::ServerSocket_sptr(
            new net::ServerSocket(opt_.pub_addr, opt_.rep_addr));
    sckt_->init();

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

        if (!(id_req = sckt_->recv()))
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
        sckt_->send(id_rep);

        clients_.push_back(new_client);

        NOTICE("Client connected - id: %i - type: %s", new_client->id(),
                clienttype_str(new_client->type()).c_str());

        delete id_req;
    }
}
