#ifndef SERVER_SERVER_HH_
# define SERVER_SERVER_HH_

# include <cstdint>
# include <list>
# include <memory>

# include <net/server-socket.hh>
# include <utils/dll.hh>

# include "client.hh"

// Forward declarations
class Options;

class Server
{
public:
    Server(const Options& opt);
    void run();

private:
    void net_init();
    void wait_for_players();

private:
    const Options& opt_;
    net::ServerSocket_sptr net_;
    utils::DLL rules_lib_;
    std::list<Client_ptr> clients_;
    uint32_t nb_clients_;
};

#endif // !SERVER_SERVER_HH_
