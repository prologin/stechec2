#ifndef SERVER_SERVER_HH_
# define SERVER_SERVER_HH_

# include <cstdint>
# include <list>
# include <memory>

// Forward declarations
namespace net { class Server; }
struct Options;
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
    // XXX: The following should be a std::unique_ptr but I have no time to find
    // out why it does not compile today
    std::shared_ptr<net::Server> net_;
    std::list<std::shared_ptr<Client>> clients_;
    unsigned nb_clients_;
};

#endif // !SERVER_SERVER_HH_
