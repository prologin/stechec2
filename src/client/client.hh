#ifndef CLIENT_CLIENT_HH_
# define CLIENT_CLIENT_HH_

# include <cstdint>
# include <memory>

# include <net/client-socket.hh>
# include <net/client-messenger.hh>
# include <rules/types.hh>
# include <rules/player.hh>
# include <utils/dll.hh>

class Options;

class Client
{
public:
    Client(const Options& opt);
    void run();

private:
    void sckt_init();
    void wait_for_game_start();

    rules::f_rules_init rules_init;
    rules::f_rules_result rules_result;
    rules::f_client_loop client_loop;

private:
    const Options& opt_;
    rules::Player_sptr player_;

    net::ClientSocket_sptr sckt_;
    net::ClientMessenger_sptr msgr_;
};

#endif // !CLIENT_CLIENT_HH_
