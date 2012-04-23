#include "server.hh"

#include <utils/log.hh>
#include <net/message.hh>
#include <rules/player.hh>

#include "options.hh"

Server::Server(const Options& opt)
    : opt_(opt),
      nb_players_(0),
      rules_lib_(std::unique_ptr<utils::DLL>(new utils::DLL(opt.rules_lib)))
{
    // Get required functions from the rules library
    rules_init = rules_lib_->get<rules::f_rules_init>("rules_init");
    server_loop = rules_lib_->get<rules::f_server_loop>("server_loop");
    rules_result = rules_lib_->get<rules::f_rules_result>("rules_result");

    players_ = rules::PlayerVector_sptr(new rules::PlayerVector());
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

    // Set the rules options
    rules::Options rules_opt;
    rules_opt.champion_lib = "";
    rules_opt.verbose = opt_.verbose;
    rules_opt.players = players_;

    // Rules specific initializations
    rules_init(rules_opt);

    // Send the server ACK to start the game
    utils::Buffer buf_gamestart;
    net::Message msg_gamestart(net::MSG_GAMESTART);
    msg_gamestart.handle_buffer(buf_gamestart);
    sckt_->push(buf_gamestart);

    // Play the game
    server_loop(msgr_);

    // Results
    rules_result();
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

    while (players_->players.size() < opt_.nb_clients)
    {
        utils::Buffer* buf_req = nullptr;

        if (!(buf_req = sckt_->recv()))
            continue;

        net::Message id_req;
        id_req.handle_buffer(*buf_req);

        if (id_req.type != net::MSG_CONNECT)
        {
            ERR("Message is not of type MSG_CONNECT, ignoring request");
            continue;
        }

        // To avoid useless message, the client_id of the request corresponds
        // to the type of the client connecting (PLAYER, SPECTATOR, ...)
        rules::Player_sptr new_player =
            rules::Player_sptr(new rules::Player(++nb_players_,
                        (rules::PlayerType) id_req.client_id));

        delete buf_req;

        // Send the reply with a uid
        net::Message id_rep(net::MSG_CONNECT, new_player->id);
        utils::Buffer buf_rep;
        id_rep.handle_buffer(buf_rep);
        sckt_->send(buf_rep);

        // Add the player to the list
        players_->players.push_back(new_player);

        NOTICE("Client connected - id: %i - type: %s", new_player->id,
                rules::playertype_str(
                    static_cast<rules::PlayerType>(new_player->type)).c_str());
    }

    // Then send players info to all clients
    utils::Buffer buf;
    net::Message msg(net::MSG_PLAYERS);
    msg.handle_buffer(buf);
    players_->handle_buffer(buf);
    sckt_->push(buf);
}
