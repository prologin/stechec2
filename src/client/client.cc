#include "client.hh"

#include <utils/log.hh>
#include <utils/buffer.hh>
#include <net/message.hh>
#include <rules/action.hh>
#include <rules/player.hh>
#include <rules/options.hh>

#include "options.hh"

Client::Client(const Options& opt)
    : opt_(opt),
      rules_lib_(std::unique_ptr<utils::DLL>(new utils::DLL(opt.rules_lib)))
{
    // Get required functions from the rules library
    rules_init = rules_lib_->get<rules::f_rules_init>("rules_init");
    rules_result = rules_lib_->get<rules::f_rules_result>("rules_result");

    if (opt_.spectator)
        client_loop = rules_lib_->get<rules::f_client_loop>("spectator_loop");
    else
        client_loop = rules_lib_->get<rules::f_client_loop>("client_loop");

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
    msgr_ = rules::ClientMessenger_sptr(new rules::ClientMessenger(sckt_));

    // Set the rules options
    rules::Options rules_opt;
    rules_opt.champion_lib = opt_.champion_lib;
    rules_opt.time = opt_.time;
    rules_opt.map_file = opt_.map_file;
    rules_opt.player = player_;
    rules_opt.verbose = opt_.verbose;
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
    utils::Buffer buf_req;
    net::Message msg(net::MSG_CONNECT, rules::PLAYER);

    if (opt_.spectator)
        msg.client_id = rules::SPECTATOR;

    msg.handle_buffer(buf_req);

    // Send the request
    utils::Buffer* buf_rep = nullptr;
    if (!sckt_->send(buf_req) || !(buf_rep = sckt_->recv()) ||
            (msg.handle_buffer(*buf_rep), msg.client_id == 0))
        FATAL("Unable to get an ID from the server");

    player_ = rules::Player_sptr(new rules::Player(msg.client_id, 0));

    delete buf_rep;

    NOTICE("Connected - id: %i", player_->id);
}

void Client::wait_for_players()
{
    utils::Buffer* buf = nullptr;
    net::Message msg;

    // Wait for players
    uint32_t msg_type = net::MSG_IGNORED;
    while (msg_type != net::MSG_PLAYERS)
    {
        buf = sckt_->pull();
        msg.handle_buffer(*buf);

        if ((msg_type = msg.type) == net::MSG_PLAYERS)
            players_->handle_buffer(*buf);

        delete buf;
    }

    // Wait for spectators
    msg_type = net::MSG_IGNORED;
    while (msg_type != net::MSG_PLAYERS)
    {
        buf = sckt_->pull();
        msg.handle_buffer(*buf);

        if ((msg_type = msg.type) == net::MSG_PLAYERS)
            spectators_->handle_buffer(*buf);

        delete buf;
    }
}

void Client::wait_for_game_start()
{
    utils::Buffer* buf = nullptr;
    net::Message msg;
    uint32_t msg_type = net::MSG_IGNORED;

    while (msg_type != net::MSG_GAMESTART)
    {
        buf = sckt_->pull();
        msg.handle_buffer(*buf);
        msg_type = msg.type;
        delete buf;
    }
}
