#ifndef LIB_NET_CLIENT_HH_
# define LIB_NET_CLIENT_HH_

# include <string>
# include <memory>

# include <net/socket.hh>

namespace net {

// Because it inherits Socket, ClientSocket's field names are pubsub* and
// reqrep* to be consistent with ServerSocket

class ClientSocket : public Socket
{
public:
    ClientSocket(const std::string& sub_addr, const std::string& req_addr);

    virtual void init();

    Message* pull();
};

typedef std::shared_ptr<ClientSocket> ClientSocket_sptr;

} // namespace net

#endif // !LIB_NET_CLIENT_HH_
