#ifndef LIB_NET_CLIENTMESSENGER_HH_
# define LIB_NET_CLIENTMESSENGER_HH_

# include <memory>
# include <stdexcept>

# include <net/messenger.hh>
# include <net/client-socket.hh>
# include <net/rules-message.hh>
# include <utils/buffer.hh>

namespace net {

class ClientMessengerError : public std::runtime_error
{
public:
    ClientMessengerError() : std::runtime_error("Client messenger error") {}
};

class ClientMessenger : public Messenger
{
public:
    ClientMessenger(ClientSocket_sptr sckt);

    virtual void send(const utils::Buffer&);
    virtual utils::Buffer* recv();
    virtual utils::Buffer* pull();

    virtual void wait_for_ack();

protected:
    virtual utils::Buffer* internal_recv(Message*);

private:
    ClientSocket_sptr sckt_;
};

typedef std::shared_ptr<ClientMessenger> ClientMessenger_sptr;

} // namespace net

#endif // !LIB_NET_CLIENTMESSENGER_HH_
