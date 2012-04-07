#include <iostream>

#include <net/client-messenger.hh>
#include <net/server-messenger.hh>

extern "C" {

void client_loop(net::ClientMessenger_sptr client_msgr)
{
    (void) client_msgr;

    std::cout << "Hello Client!" << std::endl;
}

void server_loop(net::ServerMessenger_sptr server_msgr)
{
    (void) server_msgr;

    std::cout << "Hello Server!" << std::endl;
}

} // extern "C"
