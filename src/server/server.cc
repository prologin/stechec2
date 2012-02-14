#include "server.hh"

#include <utils/log.hh>
#include <net/message.hh>
#include <net/common.hh>

#include "options.hh"
#include "client.hh"

Server::Server(const Options& opt)
    : opt_(opt)
{
}

void Server::run()
{
    // Launch the network server, listen for connections
    init();

    // We have to wait that the required number of clients specified in the
    // config is met
    wait_for_players();

    run_game();
}

void Server::init()
{
    net_ = std::unique_ptr<net::Server>(
            new net::Server(opt_.pub_addr, opt_.rep_addr));
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

        if (!(id_req = net_->get_msg()))
            continue;

        if (id_req->type != net::MSG_CONNECT)
        {
            ERR("%s", "Message is not of type MSG_CONNECT, ignoring request");
            continue;
        }

        // To avoid useless message, the client_id of the request corresponds
        // to the type of the client connecting (PLAYER, SPECTATOR, ...)
        Client_ptr new_client = Client_ptr(new Client(++nb_clients_,
                        (net::ClientType) id_req->client_id));

        net::Message id_rep(net::MSG_CONNECT, new_client->id());
        net_->send_msg(id_rep);

        clients_.push_back(new_client);

        NOTICE("Client connected - id: %i - type: %s", new_client->id(),
                clienttype_str(new_client->type()).c_str());

        delete id_req;
    }
}

void Server::run_game()
{
}
