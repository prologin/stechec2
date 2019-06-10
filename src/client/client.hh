// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef CLIENT_CLIENT_HH_
#define CLIENT_CLIENT_HH_

#include <cstdint>
#include <memory>

#include <net/client-socket.hh>
#include <rules/client-messenger.hh>
#include <rules/player.hh>
#include <rules/types.hh>
#include <utils/dll.hh>

class Client
{
public:
    Client();
    void run();

private:
    void sckt_init();
    void sckt_close();
    void wait_for_players();
    void wait_for_game_start();

    rules::f_rules_init rules_init;
    rules::f_rules_result rules_result;
    rules::f_client_loop client_loop;

private:
    std::unique_ptr<utils::DLL> rules_lib_;

    rules::Player_sptr player_;
    rules::Players_sptr players_;
    rules::Players_sptr spectators_;
    rules::ClientMessenger_sptr msgr_;

    net::ClientSocket_sptr sckt_;
};

#endif // !CLIENT_CLIENT_HH_
