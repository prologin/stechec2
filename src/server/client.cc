#include "client.hh"

Client::Client(uint32_t id, net::ClientType type)
    : id_(id),
      type_(type)
{
}
