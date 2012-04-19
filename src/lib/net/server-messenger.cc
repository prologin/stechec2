#include "server-messenger.hh"

namespace net {

ServerMessenger::ServerMessenger(ServerSocket_sptr sckt)
    : sckt_(sckt)
{
}

void ServerMessenger::send(RulesMessage* rules_msg)
{
    // TODO
}

void ServerMessenger::recv(RulesMessage* rules_msg)
{
    // TODO
}

} // namespace net
