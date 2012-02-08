#ifndef CLIENT_CLIENT_HH_
# define CLIENT_CLIENT_HH_

# include <cstdint>

namespace network {
    class Client;
}

class Client
{
public:
    void run();

private:
    void init();

private:
    network::Client* net_;
    uint32_t id_;
};

#endif // !CLIENT_CLIENT_HH_
