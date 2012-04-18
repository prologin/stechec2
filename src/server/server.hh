#ifndef SERVER_SERVER_HH_
# define SERVER_SERVER_HH_

# include <cstdint>
# include <list>
# include <memory>

# include <net/server-socket.hh>
# include <rules/types.hh>
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
    void sckt_init();
    void wait_for_players();

    rules::f_rules_init rules_init;
    rules::f_rules_turn rules_turn;
    rules::f_rules_result rules_result;

private:
    const Options& opt_;
    uint32_t nb_clients_;
    std::list<Client_ptr> clients_;

    net::ServerSocket_sptr sckt_;
};

#endif // !SERVER_SERVER_HH_
