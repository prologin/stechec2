// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#include "client.hh"

#include <gflags/gflags.h>

int main(int argc, char** argv)
{
    GFLAGS_NAMESPACE::ParseCommandLineFlags(&argc, &argv, true);

    Client client;
    client.run();
}
