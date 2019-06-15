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
    void wait_for_clients();
    std::shared_ptr<std::ostream> replay_init();
    void replay_save_results(std::shared_ptr<std::ostream> replay_stream);

    rules::f_rules_config rules_config;
    rules::f_rules_init rules_init;
    rules::f_rules_result rules_result;
    rules::f_server_loop server_loop;

private:
    std::unique_ptr<utils::DLL> rules_lib_;

    rules::Config config_;
    rules::Players players_;
    rules::Players spectators_;
    rules::ServerMessenger_sptr msgr_;

    std::unique_ptr<net::ServerSocket> sckt_;
};

#endif // !SERVER_SERVER_HH_
