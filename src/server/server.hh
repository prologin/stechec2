// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#ifndef SERVER_SERVER_HH_
#define SERVER_SERVER_HH_

#include <cstdint>
#include <list>
#include <memory>

#include <net/server-socket.hh>
#include <rules/server-messenger.hh>
#include <rules/types.hh>
#include <utils/dll.hh>

class Server
{
public:
    Server();
    void run();

private:
    void sckt_init();
    void sckt_close();
    void wait_for_players();

    rules::f_rules_init rules_init;
    rules::f_rules_result rules_result;
    rules::f_server_loop server_loop;

private:
    uint32_t nb_players_;
    std::unique_ptr<utils::DLL> rules_lib_;

    rules::Players_sptr players_;
    rules::Players_sptr spectators_;
    rules::ServerMessenger_sptr msgr_;

    std::unique_ptr<net::ServerSocket> sckt_;
};

#endif // !SERVER_SERVER_HH_
