#ifndef CLIENT_CLIENT_HH_
# define CLIENT_CLIENT_HH_

# include <cstdint>
# include <memory>

namespace net { class Client; }
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
    // XXX: The following should be a std::unique_ptr but I have no time to find
    // out why it does not compile today
    std::shared_ptr<net::Client> net_;
    uint32_t id_;
};

#endif // !CLIENT_CLIENT_HH_
