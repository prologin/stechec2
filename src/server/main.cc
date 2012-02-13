#include "server.hh"

#include "options.hh"

#include <utils/log.hh>

int main(int argc, char** argv)
{
    // Parse options
    Options opt;
    opt.process(argc, argv);

    // Init the logger
    utils::Logger::get().level() = (utils::Logger::DisplayLevel) opt.verbose;

    // The game is on, Mrs Hudson!
    Server server(opt);
    server.run();
}
