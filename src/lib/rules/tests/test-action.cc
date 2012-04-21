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

    virtual GameState* copy() const { return new MyGameState(*this); }

    int x;
};

class MyIncrAction : public Action<MyGameState>
{
public:
    virtual int check(const MyGameState* st) const
    {
        if (st->x >= 3)
            return 1;
        else
            return 0;
    }

    virtual void handle_buffer(utils::Buffer&)
    {
    }

private:
    virtual void apply_on(MyGameState* st) const
    {
        st->x += 1;
    }
};

class MyDecrAction : public Action<MyGameState>
{
public:
    virtual int check(const MyGameState* st) const
    {
        if (st->x <= 0)
            return 1;
        else
            return 0;
    }

    virtual void handle_buffer(utils::Buffer&)
    {
    }

private:
    virtual void apply_on(MyGameState* st) const
    {
        st->x -= 1;
    }
};

TEST(RulesAction, CheckApply)
{
    MyGameState* mgs = new MyGameState();
    EXPECT_EQ(0, mgs->x);

    MyIncrAction* incr = new MyIncrAction();
    MyDecrAction* decr = new MyDecrAction();

    mgs = incr->apply(mgs);
    EXPECT_EQ(1, mgs->x);

    mgs = incr->apply(mgs);
    EXPECT_EQ(2, mgs->x);

    mgs = decr->apply(mgs);
    EXPECT_EQ(1, mgs->x);

    delete incr;
    delete decr;
    delete mgs;
}

TEST(RulesAction, CheckError)
{
    MyGameState* mgs = new MyGameState();

    MyIncrAction* incr = new MyIncrAction();
    MyDecrAction* decr = new MyDecrAction();

    EXPECT_NE(0, decr->check(mgs));

    for (int i = 0; i < 3; ++i)
        mgs = incr->apply(mgs);

    EXPECT_NE(0, incr->check(mgs));

    delete incr;
    delete decr;
    delete mgs;
}

TEST(RulesAction, CheckCancel)
{
    MyGameState* mgs = new MyGameState();

    MyIncrAction* incr = new MyIncrAction();
    MyDecrAction* decr = new MyDecrAction();

    EXPECT_EQ(0, mgs->x);
    mgs = incr->apply(mgs);
    EXPECT_EQ(1, mgs->x);
    mgs = cancel(mgs);
    EXPECT_EQ(0, mgs->x);

    delete incr;
    delete decr;
    delete mgs;
}
