#include "server.hh"

#include <utils/log.hh>

int main(int, char**)
{
    utils::Logger::get().level() = utils::Logger::NOTICE_LEVEL;

    Server server;
    server.run(2);
}
