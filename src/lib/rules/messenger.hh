#ifndef LIB_RULES_MESSENGER_HH_
# define LIB_RULES_MESSENGER_HH_

# include <cstdint>
# include <net/socket.hh>
# include <net/message.hh>
# include <utils/buffer.hh>

namespace rules {

class Messenger
{
public:
    virtual ~Messenger();

    virtual void send(const utils::Buffer&, uint32_t = 0) = 0;
    virtual utils::Buffer* recv() = 0;
};

} // namespace rules

#endif // !LIB_RULES_MESSENGER_HH_
