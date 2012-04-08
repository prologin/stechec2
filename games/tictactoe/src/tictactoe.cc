#include <net/client-messenger.hh>
#include <net/server-messenger.hh>
#include <rules/player.hh>
#include <utils/log.hh>

extern "C" {

void client_init(net::ClientMessenger_sptr client_msgr)
{
    (void) client_msgr;

    INFO("client_init");
}

bool client_turn()
{
    INFO("client_turn");

    return false;
}

void client_result(rules::PlayerList* players)
{
    (void) players;

    INFO("client_result");
}

void server_init(net::ServerMessenger_sptr server_msgr)
{
    (void) server_msgr;

    INFO("server_init");
}

bool server_turn()
{
    INFO("server_turn");

    return false;
}

void server_result(rules::PlayerList* players)
{
    (void) players;

    INFO("server_result");
}

} // extern "C"
