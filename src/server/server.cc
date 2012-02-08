#include "server.hh"

#include <utils/log.hh>
#include <network/server.hh>
#include <network/message.hh>

void Server::run(unsigned nb_players)
{
    init();
    wait_for_players(nb_players);
}

void Server::init()
{
    net_ = new network::Server("tcp://127.0.0.1:2345",
                               "tcp://127.0.0.1:2346");

    net_->init();
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
            ERR("%s", "Message is not of type MSG_GETID");
            continue;
        }

        network::Message id_rep(network::MSG_GETID, nb_clients_ + 1);
        net_->send_msg(id_rep);

        delete id_req;

        INFO("One client connected, allocated id %i", nb_clients_ + 1);
    }
}
