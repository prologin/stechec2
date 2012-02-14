#ifndef SERVER_SERVER_HH_
# define SERVER_SERVER_HH_

# include <cstdint>
# include <list>
# include <memory>

# include <net/server.hh>

// Forward declarations
class Options;
class Client;
typedef std::shared_ptr<Client> Client_ptr;
typedef std::shared_ptr<Client> Client_ptr;

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
    std::list<Client_ptr> clients_;
    uint32_t nb_clients_;
};

#endif // !SERVER_SERVER_HH_
