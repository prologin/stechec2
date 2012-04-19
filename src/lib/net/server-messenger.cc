#include "server-messenger.hh"

namespace net {

ServerMessenger::ServerMessenger(ServerSocket_sptr sckt)
    : sckt_(sckt)
{
}

void ServerMessenger::send(const RulesMessage& msg)
{
    (void) msg;
    // TODO
}

uint32_t ServerMessenger::recv(void** msg)
{
    (void) msg;
    // TODO
    return 0;
}

} // namespace net
