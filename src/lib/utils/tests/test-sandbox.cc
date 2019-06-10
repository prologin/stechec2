// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#include "../sandbox.hh"

#include <gtest/gtest.h>
#include <unistd.h>

using namespace utils;

TEST(UtilsSandbox, BasicBehavior)
{
    std::function<int()> do_nothing = [] { return 0; };
    std::function<int()> sleep_2 = [] {
        sleep(2);
        return 1;
    };

    Sandbox s;

    s.execute(do_nothing);
    ASSERT_THROW(s.execute(sleep_2), SandboxTimeout);
}

TEST(UtilsSandbox, ReturnValue)
{
    std::function<int(int, int)> add = [](int x, int y) { return x + y; };

    Sandbox s;
    ASSERT_EQ(7, s.execute(add, 3, 4));
}

TEST(UtilsSandbox, SuccessiveCalls)
{
    std::function<int(int, int)> add = [](int x, int y) { return x + y; };

    Sandbox s;

    for (int i = 0; i < 10000; ++i)
        ASSERT_EQ(7, s.execute(add, 3, 4));
}

TEST(UtilsSandbox, CustomTimeout)
{
    Sandbox s(250);

    s.execute(usleep, 100000u);
    ASSERT_THROW(s.execute(usleep, 500000u), SandboxTimeout);

    s.set_timeout(80);
    ASSERT_THROW(s.execute(usleep, 1000000u), SandboxTimeout);
}

static void useless() {}

TEST(UtilsSandbox, VoidRet)
{
    std::function<void()> useless_lambda = [] {};

    Sandbox s;
    s.execute(useless);
    s.execute(useless_lambda);
}

TEST(UtilsSandbox, NoTimeout)
{
    Sandbox s(0);
    s.execute(usleep, 50000u);
}
