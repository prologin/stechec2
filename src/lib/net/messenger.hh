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
    virtual ~Messenger();

    virtual void send(const RulesMessage&) = 0;
    virtual uint32_t recv(void**) = 0;

protected:
    virtual Message* to_msg(const void* data, uint32_t data_size);
    virtual uint32_t from_msg(const Message& msg, void** data);
};

} // namespace net

#endif // !LIB_NET_MESSENGER_HH_
