#include "message.hh"

#include <sstream>
#include <ostream>

namespace network {

Message::Message(uint32_t type_, uint32_t client_id_)
    : type(type_),
      client_id(client_id_),
      size(sizeof (*this))
{
}

std::string Message::str() const
{
    std::stringstream ss;

    ss << "type      : " << type << "\n"
       << "client_id : " << client_id << "\n"
       << "size      : " << size;

    return ss.str();
}

} // namespace network

std::ostream& operator<<(std::ostream& os, const network::Message& msg)
{
    os << msg.str() << std::endl;

    return os;
}
