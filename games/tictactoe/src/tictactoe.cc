#include <net/client-messenger.hh>
#include <net/server-messenger.hh>
#include <rules/player.hh>
#include <utils/log.hh>

extern "C" {

void client_init(net::ClientMessenger_sptr client_msgr)
{
    (void) client_msgr;

    DEBUG("client_init");
}

bool client_turn()
{
    DEBUG("client_turn");

    return false;
}

void client_result(rules::PlayerList* players)
{
    (void) players;

    DEBUG("client_result");
}

void server_init(net::ServerMessenger_sptr server_msgr)
{
    (void) server_msgr;

    DEBUG("server_init");
}

bool server_turn()
{
    DEBUG("server_turn");

    return false;
}

void server_result(rules::PlayerList* players)
{
    (void) players;

    DEBUG("server_result");
}

} // extern "C"
