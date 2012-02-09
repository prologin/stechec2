#include "server.hh"

#include <utils/log.hh>
#include <network/server.hh>
#include <network/message.hh>

#include "options.hh"

Server::Server(const Options& opt)
    : opt_(opt)
{
}

void Server::run(unsigned nb_players)
{
    init();
    wait_for_players(nb_players);

    run_game();
}

void Server::init()
{
    net_ = std::shared_ptr<network::Server>(
            new network::Server(opt_.pub_addr, opt_.rep_addr));
    net_->init();

    NOTICE("Server listening on %s", opt_.rep_addr.c_str());
    NOTICE("Server publishing on %s", opt_.pub_addr.c_str());
}
void Server::wait_for_players(unsigned nb_players)
{
    for (nb_clients_ = 0; nb_clients_ < nb_players; ++nb_clients_)
    {
        network::Message* id_req = nullptr;

        if (!net_->get_msg(&id_req))
            continue;

        if (id_req->type != network::MSG_GETID)
        {
            ERR("%s", "Message is not of type MSG_GETID, ignoring request");
            continue;
        }

        network::Message id_rep(network::MSG_GETID, nb_clients_ + 1);
        net_->send_msg(id_rep);

        delete id_req;

        NOTICE("Client connected, allocated id %i", nb_clients_ + 1);
    }
}

void Server::run_game()
{
}
