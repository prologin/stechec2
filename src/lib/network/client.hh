#ifndef LIB_NETWORK_CLIENT_HH_
# define LIB_NETWORK_CLIENT_HH_

#include <string>

#include <network/socket.hh>

namespace network {

// Because it inherits Socket, Client's field names are pubsub* and reqrep* to
// be consistent with Server

class Client : public Socket
{
public:
    Client(const std::string& sub_addr, const std::string& req_addr);

    virtual void init();

private:
};

} // namespace network

#endif // !LIB_NETWORK_CLIENT_HH_
