#ifndef LIB_NETWORK_MESSAGE_HH_
# define LIB_NETWORK_MESSAGE_HH_

# include <cstdint>
# include <iostream>

namespace network {

// Message types of stechec2 (not the rules)
enum MsgType
{
    MSG_ERR = 0,
    MSG_GETID = 1
};

struct Message
{
    Message(uint32_t type_ = 0, uint32_t client_id_ = 0);

    // Type of the message (see MsgType)
    uint32_t type;

    // Id of the client that sent the packet
    // It is ignored for replies
    uint32_t client_id;

    // Size of data to send
    uint32_t size;

    // String representation of a message
    std::string str() const;
};

} // namespace network

std::ostream& operator<<(std::ostream& os, const network::Message& msg);

#endif // !LIB_NETWORK_MESSAGE_HH_
