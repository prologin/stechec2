#include <network/client.hh>

int main(int, char**)
{
    network::Client clt("tcp://localhost:2345", "tcp://localhost:2346");

    while (true)
    {
    }
}
