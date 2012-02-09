#include "server.hh"

#include "options.hh"

#include <utils/log.hh>

int main(int argc, char** argv)
{
    utils::Logger::get().level() = utils::Logger::NOTICE_LEVEL;

    Options opt;
    opt.process(argc, argv);

    Server server(opt);
    server.run(2);
}
