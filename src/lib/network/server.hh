#ifndef LIB_NETWORK_SERVER_HH_
# define LIB_NETWORK_SERVER_HH_

#include <string>

#include <network/socket.hh>

namespace network {

// Because it inherits Socket, Server's field names are pubsub* and reqrep* to
// be consistent with Client

class Server : public Socket
{
public:
    Server(const std::string& pub_addr, const std::string& rep_addr);

    void init();
};

} // namespace network

#endif // !LIB_NETWORK_SERVER_HH_
