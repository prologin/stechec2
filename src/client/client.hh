#ifndef CLIENT_CLIENT_HH_
# define CLIENT_CLIENT_HH_

# include <cstdint>
# include <memory>

# include <net/client-socket.hh>
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

private:
    const Options& opt_;
    utils::DLL rules_lib_;
    net::ClientSocket_sptr sckt_;
    uint32_t id_;
};

#endif // !CLIENT_CLIENT_HH_
