#ifndef LIB_NET_SERVERMESSENGER_HH_
# define LIB_NET_SERVERMESSENGER_HH_

# include <memory>

# include <net/messenger.hh>
# include <net/server-socket.hh>
# include <net/rules-message.hh>

namespace net {

class ServerMessenger : public Messenger
{
public:
    ServerMessenger(ServerSocket_sptr sckt);

    virtual void send(const RulesMessage&);
    virtual uint32_t recv(void** data);

private:
    ServerSocket_sptr sckt_;
};

typedef std::shared_ptr<ServerMessenger> ServerMessenger_sptr;

} // namespace net

#endif // !LIB_NET_SERVERMESSENGER_HH_
