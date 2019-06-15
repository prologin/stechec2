// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "server.hh"

#include <fstream>
#include <iostream>
#include <memory>

#include <gflags/gflags.h>

#include <net/message.hh>
#include <rules/player.hh>
#include <utils/log.hh>

DEFINE_string(rep_addr, "tcp://0.0.0.0:42124",
              "Set reply address binding (ZeroMQ)");
DEFINE_string(pub_addr, "tcp://0.0.0.0:42125",
              "Set publishing address binding (ZeroMQ)");
DEFINE_int32(time, 10000, "Timeout for a player turn (in ms)");
DEFINE_int32(nb_clients, 2, "Number of players to expect");
DEFINE_string(map, "", "Map file");
DEFINE_string(rules, "rules.so", "Rules library");
DEFINE_string(dump, "", "Game data dump output path");
DEFINE_string(replay, "", "Game replay output path");

Server::Server()
{
    rules_lib_ = std::make_unique<utils::DLL>(FLAGS_rules);
    // Get required functions from the rules library
    rules_config = rules_lib_->get<rules::f_rules_config>("rules_config");
    rules_init = rules_lib_->get<rules::f_rules_init>("rules_init");
    server_loop = rules_lib_->get<rules::f_server_loop>("server_loop");
    rules_result = rules_lib_->get<rules::f_rules_result>("rules_result");
}

void Server::run()
{
    // Launch the network server, listen for connections
    sckt_init();

    INFO("Server Started");

    // The rules configure the server
    rules_config(&config_);
    NOTICE("Game configuration:");
    NOTICE("- name: %s", config_.name);
    NOTICE("- %d players", config_.player_count);

    // We have to wait for the required number of clients specified in the
    // command line
    wait_for_clients();

    // Create a messenger for sending rules messages
    msgr_ = std::make_unique<rules::ServerMessenger>(sckt_.get());

    // Load map, if given
    auto map_content = rules::read_map_from_path(FLAGS_map);

    // Set the rules options
    rules::Options rules_opt;
    rules_opt.champion_lib = "";
    rules_opt.time = FLAGS_time;
    rules_opt.map_file = FLAGS_map;
    rules_opt.map_content = map_content;
    rules_opt.verbose = FLAGS_verbose;
    rules_opt.players = players_;
    rules_opt.spectators = spectators_;

    // Create dump output stream
    if (!FLAGS_dump.empty())
    {
        std::shared_ptr<std::ofstream> dump_stream =
            std::make_shared<std::ofstream>(FLAGS_dump);
        if (!dump_stream->is_open())
            FATAL("Cannot open dump file for writing %s: %s",
                  FLAGS_dump.c_str(), strerror(errno));
        rules_opt.dump_stream = std::move(dump_stream);
    }

    // Create replay output stream
    rules_opt.replay_stream = replay_init();

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

    // Print results
    std::cout << players_.scores_yaml();

    // Save results
    replay_save_results(rules_opt.replay_stream);

    sckt_close();
}

void Server::sckt_init()
{
    sckt_ = std::make_unique<net::ServerSocket>(FLAGS_pub_addr, FLAGS_rep_addr);
    sckt_->init();

    NOTICE("Replying on %s", FLAGS_pub_addr.c_str());
    NOTICE("Publishing on %s", FLAGS_rep_addr.c_str());
}

void Server::sckt_close()
{
    sckt_->close();
}

bool used_identifier(uint32_t player_id, const rules::Players& players)
{
    for (const auto& player : players)
        if (player->id == player_id)
            return true;
    return false;
}

void Server::wait_for_clients()
{
    if (FLAGS_nb_clients <= 0)
        FATAL("Server started with nb_clients <= 0.");

    int spectator_count = FLAGS_nb_clients - config_.player_count;
    if (spectator_count < 0)
        FATAL("Server must be started with at least -nb_clients %d",
              config_.player_count);

    NOTICE("Waiting for %d clients: %d players, %d spectators...",
           FLAGS_nb_clients, config_.player_count, spectator_count);

    // For each client connecting, we send back a unique id
    // Clients are players or spectators
    // Player IDs must be in [0-NB_PLAYER[
    // Spectator IDs must be in [NB_PLAYER-NB_CLIENTS[

    while (players_.size() + spectators_.size() <
           static_cast<size_t>(FLAGS_nb_clients))
    {
        auto buf_req = sckt_->recv();

        if (!buf_req)
            continue;

        NOTICE("New client");

        net::Message id_req;
        id_req.handle_buffer(*buf_req);

        if (id_req.type != net::MSG_CONNECT)
        {
            ERR("Message is not of type MSG_CONNECT, ignoring request");
            continue;
        }

        int32_t player_id = id_req.client_id;
        uint32_t client_type;
        buf_req->handle(client_type);

        rules::PlayerType player_type =
            static_cast<rules::PlayerType>(client_type);

        if (player_id == -1)
        {
            // Client requests an ID from the server
            if (player_type == rules::PLAYER)
                player_id = players_.size();
            else
                player_id = config_.player_count + spectators_.size();

            NOTICE("Client did not specify an identifier, picking %d",
                   player_id);
        }

        if (used_identifier(player_id, players_) ||
            used_identifier(player_id, spectators_))
        {
            ERR("Client identifier %d is already used", player_id);
            player_id = -1; // Treated by client as invalid
        }

        if (player_id < -1)
        {
            ERR("Client identifier %d is invalid", player_id);
            player_id = -1; // Treated by client as invalid
        }

        if (player_type == rules::PLAYER && player_id >= config_.player_count)
        {
            ERR("Invalid player identifier %d > %d", player_id,
                config_.player_count - 1);
            player_id = -1; // Treated by client as invalid
        }

        if (player_type == rules::SPECTATOR &&
            (player_id < config_.player_count || player_id >= FLAGS_nb_clients))
        {
            ERR("Spectator identifier %d invalid, expecting %d spectators",
                player_id, spectator_count - spectators_.size());
            player_id = -1; // Treated by client as invalid
        }

        // Send the reply with a uid
        net::Message id_rep(net::MSG_CONNECT, player_id);
        utils::Buffer buf_rep;
        id_rep.handle_buffer(buf_rep);
        sckt_->send(buf_rep);

        if (player_id == -1)
            continue; // Do not add invalid clients

        auto new_client = std::make_shared<rules::Player>(
            static_cast<uint32_t>(player_id), client_type);
        buf_req->handle(new_client->name);

        // Add the player to the list
        if (client_type == rules::SPECTATOR)
            spectators_.add(new_client);
        else
            players_.add(new_client);

        NOTICE("Client connected - id: %i - type: %s", new_client->id,
               rules::playertype_str(
                   static_cast<rules::PlayerType>(new_client->type))
                   .c_str());
    }

    players_.sort();
    spectators_.sort();

    // Then send players info to all clients
    utils::Buffer buf_players;
    net::Message msg_players(net::MSG_PLAYERS);

    msg_players.handle_buffer(buf_players);
    players_.handle_buffer(buf_players);

    sckt_->push(buf_players, 0, 500);

    // And spectators
    utils::Buffer buf_spectators;
    net::Message msg_spectators(net::MSG_PLAYERS);

    msg_spectators.handle_buffer(buf_spectators);
    spectators_.handle_buffer(buf_spectators);

    sckt_->push(buf_spectators);
}

std::shared_ptr<std::ostream> Server::replay_init()
{
    if (FLAGS_replay.empty())
        return {};

    auto ofs = std::make_shared<std::ofstream>(FLAGS_replay, std::ios::binary);
    if (!ofs->is_open())
        FATAL("Cannot open replay file for writing %s: %s",
              FLAGS_replay.c_str(), strerror(errno));

    auto map_content = rules::read_map_from_path(FLAGS_map);

    // Save state to replay file
    utils::Buffer buf;
    buf.handle(map_content);
    buf.handle_bufferizable(&players_);
    buf.handle_bufferizable(&spectators_);
    *ofs << buf;

    return ofs;
}

void Server::replay_save_results(std::shared_ptr<std::ostream> replay_stream)
{
    if (!replay_stream)
        return;
    utils::Buffer buf;
    buf.handle_bufferizable(&players_);
    *replay_stream << buf;
}
