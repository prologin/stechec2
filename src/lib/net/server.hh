#ifndef LIB_NET_SERVER_HH_
# define LIB_NET_SERVER_HH_

#include <string>

#include <net/socket.hh>

namespace net {

// Because it inherits Socket, Server's field names are pubsub* and reqrep* to
// be consistent with Client

class Server : public Socket
{
public:
    Server(const std::string& pub_addr, const std::string& rep_addr);

    virtual void init();
};

} // namespace net

#endif // !LIB_NETWORK_SERVER_HH_
