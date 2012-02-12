#include "options.hh"

#include <iostream>
#include <fstream>
#include <exception>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

void Options::process(int argc, char** argv)
{
    usage_ = std::string("Usage: ") + argv[0] + " [options]\n";
    version_ = VERSION_MESSAGE(MODULE_NAME, MODULE_VERSION);

    po::options_description cli_opt("Command-line options");
    cli_opt.add_options()
        ("help,h", "Show this help")
        ("version", "Show version")
        ("config,c", po::value<std::string>(&config),
                        "Set configuration file to use");

    po::options_description config_cli_opt("Config options");
    config_cli_opt.add_options()
        ("rep_addr,r",
            po::value<std::string>(&rep_addr)->default_value("tcp://*:4242"),
                        "Set reply address binding (ZeroMQ)")
        ("pub_addr,p",
            po::value<std::string>(&pub_addr)->default_value("tcp://*:4243"),
                        "Set publishing address binding (ZeroMQ)")
        ("nb_spectator,s",
            po::value<unsigned>(&nb_spectator)->default_value(0),
                        "Number of spectator to expect")
        ("wait_timeout,w",
            po::value<unsigned>(&wait_timeout)->default_value(1),
                        "Timeout for a player (in ms)")
        ("log,o",
            po::value<std::string>(&log)->default_value("stechec2d.log"),
                        "Output file in which to save the log")
        ("verbose,v",
            po::value<unsigned>(&verbose)->default_value(5),
                        "Verbosity of the log (0-5)")
        ("start_game_timeout,g",
            po::value<unsigned>(&start_game_timeout)->default_value(5),
                        "Timeout before the game begins (in ms)");

    po::options_description opt("");
    opt.add(cli_opt).add(config_cli_opt);

    process_cli(opt, argc, argv);

    po::options_description config_opt("Config options");
    config_opt.add_options()
        ("server.rep_addr", po::value<std::string>(&rep_addr))
        ("server.pub_addr", po::value<std::string>(&pub_addr))
        ("server.nb_spectator", po::value<unsigned>(&nb_spectator))
        ("server.wait_timeout", po::value<unsigned>(&wait_timeout))
        ("server.log", po::value<std::string>(&log))
        ("server.verbose", po::value<unsigned>(&verbose))
        ("server.start_game_timeout", po::value<unsigned>(&start_game_timeout));

    process_config(config_opt, config);
}
