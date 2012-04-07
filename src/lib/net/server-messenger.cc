#include "server-messenger.hh"

namespace net {

ServerMessenger::ServerMessenger(ServerSocket_sptr sckt)
    : sckt_(sckt)
{
}

} // namespace net
