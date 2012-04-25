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
}
