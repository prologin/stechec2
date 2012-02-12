#ifndef SERVER_OPTIONS_HH
# define SERVER_OPTIONS_HH

# include <string>

# include <utils/options.hh>

class Options : public utils::Options
{
public:
    virtual void process(int argc, char** argv);

    std::string config;
    std::string req_addr;
    std::string sub_addr;
    unsigned client_num;
    std::string rules_lib;
    std::string champion_lib;
    bool spectator;
    unsigned memory;
    unsigned time;
    std::string log;
    unsigned verbose;
};

#endif // !SERVER_OPTIONS_HH
