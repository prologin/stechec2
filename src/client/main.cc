#include "client.hh"

#include "options.hh"

#include <utils/log.hh>

int main(int argc, char** argv)
{
    Options opt;
    opt.process(argc, argv);

    utils::Logger::get().level() = (utils::Logger::DisplayLevel) opt.verbose;

    Client client(opt);
    client.run();

    INFO("Stechec2 version MODULE_VERSION_alpha");
    INFO("DO NOT USE FOR PRODUCTION IN PROLGIN 2012");
    INFO(":)");
}
