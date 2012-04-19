#ifndef LIB_NET_CLIENTMESSENGER_HH_
# define LIB_NET_CLIENTMESSENGER_HH_

# include <memory>

# include <net/messenger.hh>
# include <net/client-socket.hh>
# include <net/rules-message.hh>
# include <utils/buffer.hh>

namespace net {

class ClientMessenger : public Messenger
{
public:
    ClientMessenger(ClientSocket_sptr sckt);

    virtual void send(const utils::Buffer&);
    virtual utils::Buffer* recv();

private:
    ClientSocket_sptr sckt_;
};

typedef std::shared_ptr<ClientMessenger> ClientMessenger_sptr;

} // namespace net

#endif // !LIB_NET_CLIENTMESSENGER_HH_
