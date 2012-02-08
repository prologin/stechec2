#include <network/server.hh>
#include <network/message.hh>

int main(int, char**)
{
    network::Server srv("tcp://*:2345", "tcp://*:2346");

    while (true)
    {
        //network::Message* msg = nullptr;
        //srv.get_msg(&msg);
    }
}
