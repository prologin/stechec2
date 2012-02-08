#ifndef LIB_NETWORK_MESSAGE_HH_
# define LIB_NETWORK_MESSAGE_HH_

# include <cstdint>
# include <iostream>

namespace network {

struct Message
{
    Message(uint32_t type_ = 0, uint32_t client_id_ = 0);

    // Type of the message
    // For example synchronization packets or rule packets
    uint32_t type;

    // Id of the client that sent the packet
    // It is ignored for replies
    uint32_t client_id;

    // Size of data to send
    uint32_t size;

    // String representation of a message
    const std::string& str() const;
};

} // namespace network

std::ostream& operator<<(std::ostream& os, const network::Message& msg);

#endif // !LIB_NETWORK_MESSAGE_HH_
