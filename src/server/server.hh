#ifndef SERVER_SERVER_HH_
# define SERVER_SERVER_HH_

# include <cstdint>
# include <list>
# include <memory>

namespace network {
    class Server;
}
struct Options;

class Server
{
public:
    Server(const Options& opt);
    void run(unsigned nb_players);

private:
    void init();
    void wait_for_players(unsigned nb_players);

private:
    const Options& opt_;
    // XXX: The following should be a std::unique_ptr but I have no time to find
    // out why it does not compile today
    std::shared_ptr<network::Server> net_;
    std::list<uint32_t> clients_;
    unsigned nb_clients_;
};

#endif // !SERVER_SERVER_HH_
