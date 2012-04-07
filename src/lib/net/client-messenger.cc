#include "client-messenger.hh"

namespace net {

ClientMessenger::ClientMessenger(ClientSocket_sptr sckt)
    : sckt_(sckt)
{
}

} // namespace net
