#include <iostream>

#include <net/client-messenger.hh>

extern "C" {

void client_loop(net::ClientMessenger_sptr client_msgr)
{
    (void) client_msgr;

    std::cout << "Hello World!" << std::endl;
}

} // extern "C"
