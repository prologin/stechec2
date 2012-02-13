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

    po::variables_map vm;

    // Command line options not impacting server start
    po::options_description cli_opt("Command-line options");
    cli_opt.add_options()
        ("help,h", "Show this help")
        ("version", "Show version")
        ("config,c", po::value<std::string>(&config),
                        "Set configuration file to use");

    // Command line server options
    po::options_description config_cli_opt("Config options");
    config_cli_opt.add_options()
        ("rep_addr,r",
            po::value<std::string>(&rep_addr)->default_value("tcp://*:4242"),
                        "Set reply address binding (ZeroMQ)")
        ("pub_addr,p",
            po::value<std::string>(&pub_addr)->default_value("tcp://*:4243"),
                        "Set publishing address binding (ZeroMQ)")
        ("nb_clients,n",
            po::value<unsigned>(&nb_clients)->default_value(2),
                        "Number of players to expect")
        ("turn_timeout,t",
            po::value<unsigned>(&turn_timeout)->default_value(1),
                        "Timeout for a player turn (in ms)")
        ("log,o",
            po::value<std::string>(&log)->default_value("stechec2d.log"),
                        "Output file in which to save the log")
        ("verbose,v",
            po::value<unsigned>(&verbose)->default_value(5),
                        "Verbosity of the log (0-5)");

    po::options_description opt("");
    opt.add(cli_opt).add(config_cli_opt);

    process_cli(opt, vm, argc, argv);

    // Config file server options
    po::options_description config_opt("Config options");
    config_opt.add_options()
        ("server.rep_addr", po::value<std::string>(&rep_addr))
        ("server.pub_addr", po::value<std::string>(&pub_addr))
        ("server.nb_clients", po::value<unsigned>(&nb_clients))
        ("server.turn_timeout", po::value<unsigned>(&turn_timeout))
        ("server.log", po::value<std::string>(&log))
        ("server.verbose", po::value<unsigned>(&verbose));

    if (vm.count("config"))
        process_config(config_opt, vm, config);
}
