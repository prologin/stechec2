#ifndef SERVER_SERVER_HH_
# define SERVER_SERVER_HH_

# include <cstdint>
# include <list>
# include <memory>

# include <net/server-socket.hh>
# include <net/server-messenger.hh>
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
    rules::f_rules_result rules_result;
    rules::f_server_loop server_loop;

private:
    const Options& opt_;
    uint32_t nb_clients_;
    std::list<Client_ptr> clients_;

    net::ServerSocket_sptr sckt_;
    net::ServerMessenger_sptr msgr_;
};

#endif // !SERVER_SERVER_HH_
