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

    virtual void send(RulesMessage*) = 0;
    virtual void recv(RulesMessage*) = 0;

protected:
    virtual Message* to_msg(const uint8_t* data, uint32_t data_size);
    virtual uint32_t from_msg(const Message& msg, uint8_t** data);
};

} // namespace net

#endif // !LIB_NET_MESSENGER_HH_
