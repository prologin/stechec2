#ifndef LIB_NET_CLIENT_HH_
# define LIB_NET_CLIENT_HH_

#include <string>

#include <net/socket.hh>

namespace net {

// Because it inherits Socket, Client's field names are pubsub* and reqrep* to
// be consistent with Server

class Client : public Socket
{
public:
    Client(const std::string& sub_addr, const std::string& req_addr);

    virtual void init();

private:
};

} // namespace net

#endif // !LIB_NET_CLIENT_HH_
