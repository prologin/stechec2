#ifndef CLIENT_OPTIONS_HH
# define CLIENT_OPTIONS_HH

# include <string>
# include <cstdint>

# include <utils/options.hh>

class Options : public utils::Options
{
public:
    virtual void process(int argc, char** argv);

    // Path to the configuration file
    std::string config;

    // ZeroMQ binding for REQ socket
    std::string req_addr;

    // ZeroMQ binding for SUB socket
    std::string sub_addr;

    // Client number (to get the right section in conf file)
    unsigned client_num;

    // Path to the rules library
    std::string rules_lib;

    // Path to the champion library
    std::string champion_lib;

    // Path to the map file
    std::string map_file;

    // Is this client a spectator? (Not yet implemented)
    bool spectator;

    // Memory limit for the champion
    uint32_t memory;

    // Turn time limit for the champion
    uint32_t time;

    // Path to the file to write the logs to
    std::string log;

    // Verbosity of the logs displayed/written
    unsigned verbose;
};

#endif // !CLIENT_OPTIONS_HH
