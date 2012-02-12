#ifndef SERVER_OPTIONS_HH
# define SERVER_OPTIONS_HH

# include <string>

# include <utils/options.hh>

class Options : utils::Options
{
public:
    virtual void process(int argc, char** argv);

    std::string config;
    std::string pub_addr;
    std::string rep_addr;
    unsigned nb_spectator;
    unsigned wait_timeout;
    std::string log;
    unsigned verbose;
    unsigned start_game_timeout;
};

#endif // !SERVER_OPTIONS_HH
