// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#include "../action.hh"
#include "../game-state.hh"

#include <gtest/gtest.h>

using namespace rules;

// Test action: increments or decrements state.x in [0;3].

class MyGameState : public GameState
{
public:
    MyGameState() : GameState(), x(0) {}
    MyGameState(const MyGameState& mgs) : GameState(), x(mgs.x) {}

    GameState* copy() const override { return new MyGameState(*this); }

    int x;
};

class MyIncrAction : public Action<MyGameState>
{
public:
    ~MyIncrAction() override {}

    int check(const MyGameState* st) const override
    {
        if (st->x >= 3)
            return 1;
        else
            return 0;
    }

    void handle_buffer(utils::Buffer&) override {}

    uint32_t player_id() const override { return 0; }

    uint32_t id() const override { return 0; }

private:
    void apply_on(MyGameState* st) const override { st->x += 1; }
};

class MyDecrAction : public Action<MyGameState>
{
public:
    ~MyDecrAction() override {}

    int check(const MyGameState* st) const override
    {
        if (st->x <= 0)
            return 1;
        else
            return 0;
    }

    void handle_buffer(utils::Buffer&) override {}

    uint32_t player_id() const override { return 0; }

    uint32_t id() const override { return 0; }

private:
    void apply_on(MyGameState* st) const override { st->x -= 1; }
};

TEST(RulesAction, CheckApply)
{
    auto mgs = new MyGameState();
    EXPECT_EQ(0, mgs->x);

    MyIncrAction incr;
    MyDecrAction decr;

    mgs = incr.apply(mgs);
    EXPECT_EQ(1, mgs->x);

    mgs = incr.apply(mgs);
    EXPECT_EQ(2, mgs->x);

    mgs = decr.apply(mgs);
    EXPECT_EQ(1, mgs->x);

    delete mgs;
}

TEST(RulesAction, CheckError)
{
    auto mgs = new MyGameState();

    MyIncrAction incr;
    MyDecrAction decr;

    EXPECT_NE(0, decr.check(mgs));

    for (int i = 0; i < 3; ++i)
        mgs = incr.apply(mgs);

    EXPECT_NE(0, incr.check(mgs));

    delete mgs;
}

TEST(RulesAction, CheckCancel)
{
    auto mgs = new MyGameState();

    MyIncrAction incr;

    EXPECT_EQ(0, mgs->x);
    mgs = incr.apply(mgs);
    EXPECT_EQ(1, mgs->x);
    mgs = cancel(mgs);
    EXPECT_EQ(0, mgs->x);

    delete mgs;
}
