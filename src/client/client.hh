#ifndef CLIENT_CLIENT_HH_
# define CLIENT_CLIENT_HH_

# include <cstdint>
# include <memory>

# include <net/client-socket.hh>

class Options;

class Client
{
public:
    Client(const Options& opt);
    void run();

private:
    void init();

private:
    const Options& opt_;
    net::ClientSocket_uptr net_;
    uint32_t id_;
};

#endif // !CLIENT_CLIENT_HH_
