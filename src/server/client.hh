#ifndef SERVER_CLIENT_HH_
# define SERVER_CLIENT_HH_

# include <cstdint>

# include <net/common.hh>

// This represents a client from the Stechec server perspective

class Client
{
public:
    Client(uint32_t id, net::ClientType type);

public:
    uint32_t id() const { return id_; }
    net::ClientType type() const { return type_;}

private:
    uint32_t id_;
    net::ClientType type_;
};

#endif // !SERVER_CLIENT_HH_
