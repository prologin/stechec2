// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "client.hh"

#include <gflags/gflags.h>

#include <memory>
#include <net/message.hh>
#include <rules/action.hh>
#include <rules/options.hh>
#include <rules/player.hh>
#include <utils/buffer.hh>
#include <utils/log.hh>

DEFINE_string(name, "anonymous", "Client name (used for results)");
DEFINE_string(req_addr, "tcp://0.0.0.0:42124",
              "Set request address binding (ZeroMQ)");
DEFINE_string(sub_addr, "tcp://0.0.0.0:42125",
              "Set subscribe address binding (ZeroMQ)");
DEFINE_string(rules, "rules.so", "Rules library");
DEFINE_string(champion, "champion.so", "Champion library");
DEFINE_int32(client_id, 0, "Champion order");
DEFINE_string(map, "", "Map file");
DEFINE_bool(spectator, false, "Set if the client is a spectator");
DEFINE_int32(time, 1000, "Max time the client can use (in ms)");

Client::Client()
{
    rules_lib_ = std::make_unique<utils::DLL>(FLAGS_rules);

    // Get required functions from the rules library
    rules_init = rules_lib_->get<rules::f_rules_init>("rules_init");
    rules_result = rules_lib_->get<rules::f_rules_result>("rules_result");

    if (FLAGS_spectator)
        client_loop = rules_lib_->get<rules::f_client_loop>("spectator_loop");
    else
        client_loop = rules_lib_->get<rules::f_client_loop>("player_loop");

    players_ = rules::Players_sptr(new rules::Players());
    spectators_ = rules::Players_sptr(new rules::Players());
}

void Client::run()
{
    // Register to the stechec2 server
    sckt_init();

    // Wait for the players list to be sent by the server
    wait_for_players();

    // Create a messenger for sending rules messages
    msgr_ = std::make_unique<rules::ClientMessenger>(sckt_.get(), player_->id);

    // Load map, if given
    auto map_content = rules::read_map_from_path(FLAGS_map);

    // Set the rules options
    rules::Options rules_opt;
    rules_opt.champion_lib = FLAGS_champion;
    rules_opt.time = FLAGS_time;
    rules_opt.map_file = FLAGS_map;
    rules_opt.map_content = map_content;
    rules_opt.player = player_;
    rules_opt.verbose = FLAGS_verbose;
    rules_opt.players = players_;
    rules_opt.spectators = spectators_;

    // Rules specific initializations
    rules_init(rules_opt);

    // Wait for the server ACK to start the game
    wait_for_game_start();

    // Play the game
    client_loop(msgr_);

    // Results
    rules_result();

    sckt_close();
}

void Client::sckt_init()
{
    sckt_ = std::make_unique<net::ClientSocket>(FLAGS_sub_addr, FLAGS_req_addr);
    sckt_->init();

    NOTICE("Requesting on %s", FLAGS_req_addr.c_str());
    NOTICE("Subscribing on %s", FLAGS_sub_addr.c_str());

    // Compute the client type
    uint32_t client_type;
    if (FLAGS_spectator)
        client_type = rules::SPECTATOR;
    else
        client_type = rules::PLAYER;

    // Send a message to get an ID from the server
    // To avoid useless message, the client_id of the request corresponds
    // to the type of the client connecting (PLAYER, SPECTATOR, ...)
    // and its requested identifier.
    int id_and_type = client_type + rules::MAX_PLAYER_TYPE * FLAGS_client_id;
    utils::Buffer buf_req;
    net::Message msg(net::MSG_CONNECT, id_and_type);

    msg.handle_buffer(buf_req);
    buf_req.handle(FLAGS_name);

    // Send the request
    std::unique_ptr<utils::Buffer> buf_rep;
    if (!sckt_->send(buf_req) || !(buf_rep = sckt_->recv()) ||
        (msg.handle_buffer(*buf_rep), msg.client_id == 0))
        FATAL("Unable to get an ID from the server");

    player_ = rules::Player_sptr(new rules::Player(msg.client_id, client_type));
    player_->name = FLAGS_name;

    NOTICE("Connected - id: %i", player_->id);
}

void Client::sckt_close()
{
    sckt_->close();
}

void Client::wait_for_players()
{
    net::Message msg;

    // Wait for players
    uint32_t msg_type = net::MSG_IGNORED;
    while (msg_type != net::MSG_PLAYERS)
    {
        auto buf = sckt_->pull();
        msg.handle_buffer(*buf);

        if ((msg_type = msg.type) == net::MSG_PLAYERS)
            players_->handle_buffer(*buf);
    }

    // Wait for spectators
    msg_type = net::MSG_IGNORED;
    while (msg_type != net::MSG_PLAYERS)
    {
        auto buf = sckt_->pull();
        msg.handle_buffer(*buf);

        if ((msg_type = msg.type) == net::MSG_PLAYERS)
            spectators_->handle_buffer(*buf);
    }
}

void Client::wait_for_game_start()
{
    net::Message msg;
    uint32_t msg_type = net::MSG_IGNORED;

    while (msg_type != net::MSG_GAMESTART)
    {
        auto buf = sckt_->pull();
        msg.handle_buffer(*buf);
        msg_type = msg.type;
    }
}
