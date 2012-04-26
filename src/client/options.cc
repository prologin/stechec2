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

    po::options_description cli_opt("Command-line options");
    cli_opt.add_options()
        ("help,h", "Show this help")
        ("version", "Show version")
        ("config,c", po::value<std::string>(&config),
                        "Set configuration file to use")
        ("client_name,n",
            po::value<std::string>(&client_name)->default_value("anonymous"),
                        "Client name (used for the configuration file and results)");

    po::options_description config_cli_opt("Config options");
    config_cli_opt.add_options()
        ("req_addr,r",
            po::value<std::string>(&req_addr)->default_value("tcp://*:42124"),
                        "Set request address binding (ZeroMQ)")
        ("sub_addr,p",
            po::value<std::string>(&sub_addr)->default_value("tcp://*:42125"),
                        "Set subscribe address binding (ZeroMQ)")
        ("rules,u",
            po::value<std::string>(&rules_lib)->default_value("rules.so"),
                        "Rules library")
        ("champion,a",
            po::value<std::string>(&champion_lib)->default_value("champion.so"),
                        "Champion library")
        ("map,f",
            po::value<std::string>(&map_file)->default_value("default.map"),
                        "Map file")
        ("spectator,s",
            po::value<bool>(&spectator)->default_value(false),
                        "Set if the client is a spectator")
        ("memory,m",
            po::value<unsigned>(&memory)->default_value(42),
                        "Max memory the client can use (in MiB)")
        ("time,t",
            po::value<unsigned>(&time)->default_value(1000),
                        "Max time the client can use (in ms)")
        ("log,o",
            po::value<std::string>(&log)->default_value("stechec2c.log"),
                        "Output file in which to save the log")
        ("verbose,v",
            po::value<unsigned>(&verbose)->default_value(5),
                        "Verbosity of the log (0-5)");

    po::options_description opt("");
    opt.add(cli_opt).add(config_cli_opt);

    process_cli(opt, vm, argc, argv);

    std::string client_str = "stechec2.client." + client_name;

    po::options_description config_opt("Config options");
    config_opt.add_options()
        ((client_str + ".req_addr").c_str(),
            po::value<std::string>(&req_addr))
        ((client_str + ".sub_addr").c_str(),
            po::value<std::string>(&sub_addr))
        ((client_str + ".rules").c_str(),
            po::value<std::string>(&rules_lib))
        ((client_str + ".champion").c_str(),
            po::value<std::string>(&champion_lib))
        ((client_str + ".map").c_str(),
            po::value<std::string>(&map_file))
        ((client_str + ".spectator").c_str(),
            po::value<bool>(&spectator))
        ((client_str + ".memory").c_str(),
            po::value<unsigned>(&memory))
        ((client_str + ".time").c_str(),
            po::value<unsigned>(&time))
        ((client_str + ".log").c_str(),
            po::value<std::string>(&log))
        ((client_str + ".verbose").c_str(),
            po::value<unsigned>(&verbose));

    if (vm.count("config"))
        process_config(config_opt, vm, config);
}
