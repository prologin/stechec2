#include "client-messenger.hh"

namespace net {

ClientMessenger::ClientMessenger(ClientSocket_sptr sckt)
    : sckt_(sckt)
{
}

void ClientMessenger::send(const RulesMessage& msg)
{
    (void) msg;
    // TODO
}

uint32_t ClientMessenger::recv(void** msg)
{
    (void) msg;
    // TODO
    return 0;
}

} // namespace net
