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
        ("client_num,n",
            po::value<unsigned>(&client_num)->default_value(1),
                        "Client number (to get the right section in conf file");

    po::options_description config_cli_opt("Config options");
    config_cli_opt.add_options()
        ("req_addr,r",
            po::value<std::string>(&req_addr)->default_value("tcp://*:42124"),
                        "Set request address binding (ZeroMQ)")
        ("sub_addr,p",
            po::value<std::string>(&sub_addr)->default_value("tcp://*:42125"),
                        "Set subscribe address binding (ZeroMQ)")
        ("rules,u",
            po::value<std::string>(&rules_lib)->default_value("./rules.so"),
                        "Rules library")
        ("champion,a",
            po::value<std::string>(&champion_lib)->default_value("./champion.so"),
                        "Champion library")
        ("map,f",
            po::value<std::string>(&map_file)->default_value("default.map"),
                        "Map file")
        ("spectator,s",
            po::value<bool>(&spectator)->default_value(false),
                        "Set if the client is a spectator")
        ("memory,m",
            po::value<unsigned>(&memory)->default_value(1000),
                        "Max memory the client can use (in KiB)")
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

    std::stringstream client_num_ss;
    client_num_ss << client_num;
    std::string client_num_str = "client." + client_num_ss.str();

    po::options_description config_opt("Config options");
    config_opt.add_options()
        ((client_num_str + ".req_addr").c_str(),
            po::value<std::string>(&req_addr))
        ((client_num_str + ".sub_addr").c_str(),
            po::value<std::string>(&sub_addr))
        ((client_num_str + ".rules").c_str(),
            po::value<std::string>(&rules_lib))
        ((client_num_str + ".champion").c_str(),
            po::value<std::string>(&champion_lib))
        ((client_num_str + ".map").c_str(),
            po::value<std::string>(&map_file))
        ((client_num_str + ".spectator").c_str(),
            po::value<bool>(&spectator))
        ((client_num_str + ".memory").c_str(),
            po::value<unsigned>(&memory))
        ((client_num_str + ".time").c_str(),
            po::value<unsigned>(&time))
        ((client_num_str + ".log").c_str(),
            po::value<std::string>(&log))
        ((client_num_str + ".verbose").c_str(),
            po::value<unsigned>(&verbose));

    if (vm.count("config"))
        process_config(config_opt, vm, config);
}
