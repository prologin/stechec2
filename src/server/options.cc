#include "options.hh"

#include <iostream>
#include <exception>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

void Options::process(int argc, char** argv)
{
    std::string usage =
        std::string("Usage: ") + argv[0] + " [options]\n";

    po::options_description cli_opt("CLI options");
    cli_opt.add_options()
        ("help,h", "Show this help")
        ("version,v", "Show version");

    po::options_description config_opt("Config options");
    config_opt.add_options()
        ("config,c", po::value<std::string>(&config)->default_value(""),
                        "Set configuration file to use")
        ("pub_addr,p",
            po::value<std::string>(&pub_addr)->default_value("tcp://*:4242"),
                        "Set publishing address binding (ZeroMQ style)")
        ("rep_addr,r",
            po::value<std::string>(&rep_addr)->default_value("tcp://*:4243"),
                        "Set reply address binding (ZeroMQ style)")
        ("nb_spectator,s",
            po::value<unsigned>(&nb_spectator)->default_value(0),
                        "Number of spectator to expect")
        ("wait_timeout,w",
            po::value<unsigned>(&wait_timeout)->default_value(1),
                        "Timeout for a player (in milliseconds)")
        ("log,o", po::value<std::string>(&log)->default_value("stechec2.log"),
                        "Output file in which to save the log")
        ("verbose,d", po::value<unsigned>(&verbose)->default_value(3),
                        "Verbosity of the log (0-3)")
        ("start_game_timeout,g",
            po::value<unsigned>(&start_game_timeout)->default_value(5),
                        "Timeout before the game begins (in milliseconds)");

    po::options_description opt("All options");
    opt.add(cli_opt).add(config_opt);

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
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n' << usage << opt << std::endl;
        exit(1);
    }
}
