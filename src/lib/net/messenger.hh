#ifndef LIB_NET_MESSENGER_HH_
# define LIB_NET_MESSENGER_HH_

# include <cstdint>

# include <net/socket.hh>
# include <net/message.hh>
# include <net/rules-message.hh>

namespace net {

class Messenger
{
public:
    Messenger(Socket* sckt);
    virtual ~Messenger();

protected:
    virtual Message* to_msg(const void* data, uint32_t data_size);
    virtual uint32_t from_msg(const Message& msg, void** data);

protected:
    Socket* sckt_;
};

} // namespace net

#endif // !LIB_NET_MESSENGER_HH_
