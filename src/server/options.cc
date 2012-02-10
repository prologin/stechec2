#include "options.hh"

#include <iostream>
#include <fstream>
#include <exception>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

void Options::process(int argc, char** argv)
{
    std::string usage =
        std::string("Usage: ") + argv[0] + " [options]\n";

    po::options_description cli_opt("Command-line options");
    cli_opt.add_options()
        ("help,h", "Show this help")
        ("version,v", "Show version")
        ("config,c", po::value<std::string>(&config),
                        "Set configuration file to use");

    po::options_description config_cli_opt("Config options");
    config_cli_opt.add_options()
        ("rep_addr,r,server.rep_addr",
            po::value<std::string>(&rep_addr)->default_value("tcp://*:4242"),
                        "Set reply address binding (ZeroMQ)")
        ("pub_addr,p,server.pub_addr",
            po::value<std::string>(&pub_addr)->default_value("tcp://*:4243"),
                        "Set publishing address binding (ZeroMQ)")
        ("nb_spectator,s,server.nb_spectator",
            po::value<unsigned>(&nb_spectator)->default_value(0),
                        "Number of spectator to expect")
        ("wait_timeout,w,server.wait_timeout",
            po::value<unsigned>(&wait_timeout)->default_value(1),
                        "Timeout for a player (in milliseconds)")
        ("log,o,server.log",
            po::value<std::string>(&log)->default_value("stechec2.log"),
                        "Output file in which to save the log")
        ("verbose,d,server.verbose",
            po::value<unsigned>(&verbose)->default_value(5),
                        "Verbosity of the log (0-5)")
        ("start_game_timeout,g,server.start_game_timeout",
            po::value<unsigned>(&start_game_timeout)->default_value(5),
                        "Timeout before the game begins (in milliseconds)");

    po::options_description config_opt("Config options");
    config_opt.add_options()
        ("server.rep_addr", po::value<std::string>(&rep_addr))
        ("server.pub_addr", po::value<std::string>(&pub_addr))
        ("server.nb_spectator", po::value<unsigned>(&nb_spectator))
        ("server.wait_timeout", po::value<unsigned>(&wait_timeout))
        ("server.log", po::value<std::string>(&log))
        ("server.verbose", po::value<unsigned>(&verbose))
        ("server.start_game_timeout", po::value<unsigned>(&start_game_timeout));

    po::options_description opt("");
    opt.add(cli_opt).add(config_cli_opt);

    try
    {
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(opt).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << usage << opt << std::endl;
            exit(0);
        }

        if (vm.count("version"))
        {
            std::cout << "Stechec2 " << MODULE_NAME
                      << " v" << MODULE_VERSION << "\n"
                      << "Copyright (c) 2012 Prologin\n\n"
                      << "Written by Pierre Bourdon <delroth@gmail.com>\n"
                      << "       and Nicolas Hureau <nh@kalenz.fr>"
                      << std::endl;
            exit(0);
        }

        if (vm.count("config"))
        {
            std::ifstream input_file(config);

            if (!input_file.is_open())
                throw std::runtime_error("Could not open config file \"" +
                        config + "\"");
            po::store(po::parse_config_file(input_file, config_opt, true), vm);
            po::notify(vm);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n\n" << usage << opt << std::endl;
        exit(1);
    }
}
