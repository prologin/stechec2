#ifndef LIB_NET_SERVERMESSENGER_HH_
# define LIB_NET_SERVERMESSENGER_HH_

# include <memory>

# include <net/messenger.hh>
# include <net/server-socket.hh>
# include <net/rules-message.hh>
# include <utils/buffer.hh>

namespace net {

class ServerMessenger : public Messenger
{
public:
    ServerMessenger(ServerSocket_sptr sckt);

    virtual void send(const utils::Buffer&);
    virtual utils::Buffer* recv();

private:
    ServerSocket_sptr sckt_;
};

typedef std::shared_ptr<ServerMessenger> ServerMessenger_sptr;

} // namespace net

#endif // !LIB_NET_SERVERMESSENGER_HH_
