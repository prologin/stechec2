// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
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

    std::shared_ptr<rules::Player> player_;
    rules::Players players_;
    rules::Players spectators_;
    rules::ClientMessenger_sptr msgr_;

    std::unique_ptr<net::ClientSocket> sckt_;
};

#endif // !CLIENT_CLIENT_HH_
