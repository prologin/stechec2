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
            po::value<std::string>(&rep_addr)->default_value("tcp://0.0.0.0:42124"),
                        "Set reply address binding (ZeroMQ)")
        ("pub_addr,p",
            po::value<std::string>(&pub_addr)->default_value("tcp://0.0.0.0:42125"),
                        "Set publishing address binding (ZeroMQ)")
        ("rules,u",
            po::value<std::string>(&rules_lib)->default_value("rules.so"),
                        "Rules library")
        ("map,f",
            po::value<std::string>(&map_file)->default_value("default.map"),
                        "Map file")
        ("nb_clients,n",
            po::value<uint32_t>(&nb_clients)->default_value(2),
                        "Number of players to expect")
        ("turn_timeout,t",
            po::value<uint32_t>(&turn_timeout)->default_value(10000),
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
        ("stechec2.server.rep_addr", po::value<std::string>(&rep_addr))
        ("stechec2.server.pub_addr", po::value<std::string>(&pub_addr))
        ("stechec2.server.rules", po::value<std::string>(&rules_lib))
        ("stechec2.server.map", po::value<std::string>(&map_file))
        ("stechec2.server.nb_clients", po::value<unsigned>(&nb_clients))
        ("stechec2.server.turn_timeout", po::value<unsigned>(&turn_timeout))
        ("stechec2.server.log", po::value<std::string>(&log))
        ("stechec2.server.verbose", po::value<unsigned>(&verbose));

    if (vm.count("config"))
        process_config(config_opt, vm, config);
}
