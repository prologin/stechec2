#ifndef LIB_NET_MESSENGER_HH_
# define LIB_NET_MESSENGER_HH_

# include <cstdint>
# include <net/socket.hh>
# include <net/message.hh>
# include <net/rules-message.hh>
# include <utils/buffer.hh>

namespace net {

class Messenger
{
public:
    virtual ~Messenger();

    virtual void send(const utils::Buffer&) = 0;
    virtual utils::Buffer* recv() = 0;
};

} // namespace net

#endif // !LIB_NET_MESSENGER_HH_
