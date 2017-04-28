#include "server.hh"

#include <fstream>
#include <gflags/gflags.h>
#include <iostream>
#include <memory>
#include <net/message.hh>
#include <rules/player.hh>
#include <utils/log.hh>

DEFINE_string(rep_addr, "tcp://0.0.0.0:42124",
              "Set reply address binding (ZeroMQ)");
DEFINE_string(pub_addr, "tcp://0.0.0.0:42125",
              "Set publishing address binding (ZeroMQ)");
DEFINE_int32(time, 10000, "Timeout for a player turn (in ms)");
DEFINE_int32(nb_clients, 2, "Number of players to expect");
DEFINE_string(map, "default.map", "Map file");
DEFINE_string(rules, "rules.so", "Rules library");
DEFINE_string(dump, "", "Game data dump output path");

Server::Server()
    : nb_players_(0)
{
    rules_lib_.reset(new utils::DLL(FLAGS_rules));
    // Get required functions from the rules library
    rules_init = rules_lib_->get<rules::f_rules_init>("rules_init");
    server_loop = rules_lib_->get<rules::f_server_loop>("server_loop");
    rules_result = rules_lib_->get<rules::f_rules_result>("rules_result");

    players_ = rules::Players_sptr(new rules::Players());
    spectators_ = rules::Players_sptr(new rules::Players());
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
    msgr_ = rules::ServerMessenger_sptr(new rules::ServerMessenger(sckt_));

    // Set the rules options
    rules::Options rules_opt;
    rules_opt.champion_lib = "";
    rules_opt.time = FLAGS_time;
    rules_opt.map_file = FLAGS_map;
    rules_opt.verbose = FLAGS_verbose;
    rules_opt.players = players_;
    rules_opt.spectators = spectators_;

    // Create dump output stream
    if (!FLAGS_dump.empty())
    {
        std::shared_ptr<std::ofstream> dump_stream =
            std::make_shared<std::ofstream>(FLAGS_dump);
        if (!dump_stream->is_open())
            ERR("Cannot open dump file for writing %s: %s",
                    FLAGS_dump.c_str(), strerror(errno));
        else
            rules_opt.dump_stream = std::move(dump_stream);
    }

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

    for (auto player : players_->players)
    {
        std::cout << "---" << std::endl
                  << "player: " << player->name.c_str() << std::endl
                  << "score: " << player->score << std::endl
                  << "nb_timeout: " << player->nb_timeout << std::endl;
    }

    sckt_close();
}

void Server::sckt_init()
{
    sckt_ = net::ServerSocket_sptr(
            new net::ServerSocket(FLAGS_pub_addr, FLAGS_rep_addr));
    sckt_->init();

    NOTICE("Replying on %s", FLAGS_pub_addr.c_str());
    NOTICE("Publishing on %s", FLAGS_rep_addr.c_str());
}

void Server::sckt_close()
{
    sckt_->close();
}

bool used_identifier(uint player_id, rules::Players_sptr ps)
{
    for (rules::Player_sptr p : ps->players)
        if (p->id == player_id)
            return true;
    return false;
}

void Server::wait_for_players()
{
    // For each client connecting, we send back a unique id
    // Clients are players or spectators

    while (players_->size() + spectators_->size() <
            static_cast<size_t>(FLAGS_nb_clients))
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
        // and the requested client identifier.
        int id_and_type = id_req.client_id;
        int player_id = id_and_type / rules::MAX_PLAYER_TYPE;
        rules::PlayerType player_type =
          static_cast<rules::PlayerType>(id_and_type % rules::MAX_PLAYER_TYPE);

        ++nb_players_;
        if (player_id == 0)
        {
            player_id = nb_players_;
            NOTICE("Client did not request an identifier, defaulting to %i",
                   player_id);
        }

        if (used_identifier(player_id, players_)
            || used_identifier(player_id, spectators_))
        {
            ERR("Client identifier %i is already used", player_id);
            player_id = 0; // Treated by client as invalid
        }

        rules::Player_sptr new_player =
          rules::Player_sptr(new rules::Player(player_id, player_type));
        buf_req->handle(new_player->name);

        delete buf_req;

        // Send the reply with a uid
        net::Message id_rep(net::MSG_CONNECT, new_player->id);
        utils::Buffer buf_rep;
        id_rep.handle_buffer(buf_rep);
        sckt_->send(buf_rep);

        // Add the player to the list
        if (new_player->type == rules::SPECTATOR)
            spectators_->players.push_back(new_player);
        else
            players_->players.push_back(new_player);

        NOTICE("Client connected - id: %i - type: %s", new_player->id,
                rules::playertype_str(
                    static_cast<rules::PlayerType>(new_player->type)).c_str());
    }

    // Sort players by identifiers
    std::sort(players_->players.begin(), players_->players.end(),
              [] (rules::Player_sptr const& a, rules::Player_sptr const& b) {
                return a->id < b->id;
              });

    // Then send players info to all clients
    utils::Buffer buf_players;
    net::Message msg_players(net::MSG_PLAYERS);

    msg_players.handle_buffer(buf_players);
    players_->handle_buffer(buf_players);

    sckt_->push(buf_players, 0, 500);

    // And spectators
    utils::Buffer buf_spectators;
    net::Message msg_spectators(net::MSG_PLAYERS);

    msg_spectators.handle_buffer(buf_spectators);
    spectators_->handle_buffer(buf_spectators);

    sckt_->push(buf_spectators);
}
