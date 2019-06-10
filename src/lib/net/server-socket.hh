#ifndef LIB_NET_SERVER_HH_
#define LIB_NET_SERVER_HH_

#include <memory>
#include <string>
#include <utils/buffer.hh>

#include <net/message.hh>
#include <net/socket.hh>

namespace net {

// Because it inherits Socket, ServerSocket's field names are pubsub* and
// reqrep* to be consistent with ClientSocket

class ServerSocket : public Socket
{
public:
    ServerSocket(const std::string& pub_addr, const std::string& rep_addr);

    void init() override;
    void close() override;

    bool push(const utils::Buffer& buf, int flags = 0, size_t sleep = 0);
};

typedef std::shared_ptr<ServerSocket> ServerSocket_sptr;

} // namespace net

#endif // !LIB_NETWORK_SERVER_HH_
