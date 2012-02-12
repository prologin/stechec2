#include "options.hh"

#include <fstream>

namespace po = boost::program_options;

namespace utils {

void Options::process_cli(po::options_description opt,
                          int argc, char** argv)
{
    try
    {
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(opt).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << usage_ << opt << std::endl;
            exit(0);
        }

        if (vm.count("version"))
        {
            std::cout << version_ << std::endl;
            exit(0);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n\n" << usage_ << opt << std::endl;
        exit(1);
    }
}

void Options::process_config(po::options_description opt,
                             const std::string& config_file)
{
    try
    {
        std::ifstream input_file(config_file);

        if (!input_file.is_open())
            throw std::runtime_error("Could not open config file \"" +
                    config_file + "\"");

        po::variables_map vm;
        po::store(po::parse_config_file(input_file, opt, true), vm);
        po::notify(vm);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n\n" << usage_ << opt << std::endl;
        exit(1);
    }
}

} // namespace utils
