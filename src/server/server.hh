#ifndef SERVER_SERVER_HH_
# define SERVER_SERVER_HH_

# include <cstdint>
# include <list>

namespace network {
    class Server;
}

class Server
{
public:
    void run(unsigned nb_players);

private:
    void init();
    void wait_for_players(unsigned nb_players);

private:
    network::Server* net_;
    std::list<uint32_t> clients_;
    unsigned nb_clients_;
};

#endif // !SERVER_SERVER_HH_
