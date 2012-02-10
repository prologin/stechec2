#ifndef CLIENT_CLIENT_HH_
# define CLIENT_CLIENT_HH_

# include <cstdint>

namespace net {
    class Client;
}

class Client
{
public:
    void run();

private:
    void init();

private:
    net::Client* net_;
    uint32_t id_;
};

#endif // !CLIENT_CLIENT_HH_
