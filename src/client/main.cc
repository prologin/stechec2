#include "client.hh"

#include <utils/log.hh>

int main(int, char**)
{
    utils::Logger::get().level() = utils::Logger::NOTICE_LEVEL;

    Client client;
    client.run();
}
