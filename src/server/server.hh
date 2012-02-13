#ifndef SERVER_SERVER_HH_
# define SERVER_SERVER_HH_

# include <cstdint>
# include <list>
# include <memory>

# include <net/server.hh>

// Forward declarations
class Options;
class Client;

class Server
{
public:
    Server(const Options& opt);
    void run();

private:
    void init();
    void wait_for_players();
    void run_game();

private:
    const Options& opt_;
    std::unique_ptr<net::Server> net_;
    std::list<std::shared_ptr<Client>> clients_;
    unsigned nb_clients_;
};

#endif // !SERVER_SERVER_HH_
