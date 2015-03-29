#include "server.hh"

#include <gflags/gflags.h>

int main(int argc, char** argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // The game is on, Mrs Hudson!
    Server server;
    server.run();
}
