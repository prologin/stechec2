#include "../options.hh"

#include <gtest/gtest.h>
#include <boost/program_options.hpp>
#include <cstring>

using namespace utils;

namespace po = boost::program_options;

class MyOptions : public Options
{
public:
    virtual void process(int argc, char** argv)
    {
        usage_ = std::string("Usage: ") + argv[0] + " [options]\n";
        version_ = VERSION_MESSAGE("test", "42");

        po::variables_map vm;

        po::options_description cli_test_opt("Command-line test options");
        cli_test_opt.add_options()
            ("test-cli,t", po::value<int>(&test_cli)->default_value(42), "Tst")
            ("config,c", po::value<std::string>(&config));

        process_cli(cli_test_opt, vm, argc, argv);

        po::options_description config_test_opt("Config test options");
        config_test_opt.add_options()
            ("config.test",
                po::value<std::string>(&config_test)->default_value("tst"));

        if (vm.count("config"))
            process_config(config_test_opt, vm, config);
    }

    int test_cli;
    std::string config;
    std::string config_test;
};

TEST(UtilsOptions, CLI_success)
{
    int argv_size = 10;
    int argv_elt_size = 50;

    MyOptions opt;

    char** argv = new char*[argv_size];
    for (int i = 0; i < argv_size; ++i)
        argv[i] = new char[argv_elt_size];

    // ./test
    strcpy(argv[0], "./test");
    opt.process(1, argv);
    EXPECT_EQ(opt.test_cli, 42);
    EXPECT_EQ(opt.config, "");

    // ./test -t 21
    strcpy(argv[1], "-t");
    strcpy(argv[2], "21");
    opt.process(3, argv);
    EXPECT_EQ(opt.test_cli, 21);
    EXPECT_EQ(opt.config, "");

    for (int i = 0; i < argv_size; ++i)
        delete[] argv[i];
    delete[] argv;
}
