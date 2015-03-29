#include "client.hh"

#include <gflags/gflags.h>

int main(int argc, char** argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    Client client;
    client.run();
}
