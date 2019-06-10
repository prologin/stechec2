#ifndef LIB_NET_CLIENT_HH_
#define LIB_NET_CLIENT_HH_

#include <memory>
#include <string>
#include <utils/buffer.hh>

#include <net/socket.hh>

namespace net {

// Because it inherits Socket, ClientSocket's field names are pubsub* and
// reqrep* to be consistent with ServerSocket

class ClientSocket : public Socket
{
public:
    ClientSocket(const std::string& sub_addr, const std::string& req_addr);

    void init() override;
    void close() override;

    utils::Buffer* pull(int flags = 0);
};

typedef std::shared_ptr<ClientSocket> ClientSocket_sptr;

} // namespace net

#endif // !LIB_NET_CLIENT_HH_
