#ifndef LIB_NET_SERVER_HH_
# define LIB_NET_SERVER_HH_

# include <string>
# include <memory>

# include <net/socket.hh>

namespace net {

// Because it inherits Socket, ServerSocket's field names are pubsub* and
// reqrep* to be consistent with ClientSocket

class ServerSocket : public Socket
{
public:
    ServerSocket(const std::string& pub_addr, const std::string& rep_addr);

    virtual void init();
};

typedef std::unique_ptr<ServerSocket> ServerSocket_uptr;

} // namespace net

#endif // !LIB_NETWORK_SERVER_HH_
