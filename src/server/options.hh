#ifndef SERVER_OPTIONS_HH
# define SERVER_OPTIONS_HH

# include <string>

# include <utils/options.hh>

class Options : utils::Options
{
public:
    virtual void process(int argc, char** argv);

    // Path to the configuration file
    std::string config;

    // ZeroMQ binding for REP socket
    std::string rep_addr;

    // ZeroMQ binding for PUB socket
    std::string pub_addr;

    // Path to the rules library
    std::string rules_lib;

    // Number of clients the server has to expect before beginning a game
    unsigned nb_clients;

    // Time a player has to play a turn before he timeouts
    unsigned turn_timeout;

    // Path to the file to write the logs to
    std::string log;

    // Verbosity of the logs displayed/written
    unsigned verbose;
};

#endif // !SERVER_OPTIONS_HH
