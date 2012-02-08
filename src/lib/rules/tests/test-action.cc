#include "../action.hh"
#include "../state.hh"

#include <gtest/gtest.h>

// Test action: increments or decrements state.x in [0;3].

class MyRulesState : public RulesState
{
public:
    MyRulesState() : RulesState(), x(0) {}
    MyRulesState(const MyRulesState& mrs) : RulesState(), x(mrs.x) {}

    virtual RulesState* copy() const { return new MyRulesState(*this); }

    int x;
};

class MyIncrAction : public RulesAction<MyRulesState>
{
public:
    virtual int check(const MyRulesState* st) const
    {
        if (st->x >= 3)
            return 1;
        else
            return 0;
    }

    virtual void serialize(Serializer&) const
    {
    }

private:
    virtual void apply_on(MyRulesState* st) const
    {
        st->x += 1;
    }
};

class MyDecrAction : public RulesAction<MyRulesState>
{
public:
    virtual int check(const MyRulesState* st) const
    {
        if (st->x <= 0)
            return 1;
        else
            return 0;
    }

    virtual void serialize(Serializer&) const
    {
    }

private:
    virtual void apply_on(MyRulesState* st) const
    {
        st->x -= 1;
    }
};

TEST(RulesAction, CheckApply)
{
    MyRulesState* mrs = new MyRulesState();
    EXPECT_EQ(0, mrs->x);

    MyIncrAction* incr = new MyIncrAction();
    MyDecrAction* decr = new MyDecrAction();

    mrs = incr->apply(mrs);
    EXPECT_EQ(1, mrs->x);

    mrs = incr->apply(mrs);
    EXPECT_EQ(2, mrs->x);

    mrs = decr->apply(mrs);
    EXPECT_EQ(1, mrs->x);

    delete incr;
    delete decr;
    delete mrs;
}

TEST(RulesAction, CheckError)
{
    MyRulesState* mrs = new MyRulesState();

    MyIncrAction* incr = new MyIncrAction();
    MyDecrAction* decr = new MyDecrAction();

    EXPECT_NE(0, decr->check(mrs));

    for (int i = 0; i < 3; ++i)
        mrs = incr->apply(mrs);

    EXPECT_NE(0, incr->check(mrs));

    delete incr;
    delete decr;
    delete mrs;
}

TEST(RulesAction, CheckCancel)
{
    MyRulesState* mrs = new MyRulesState();

    MyIncrAction* incr = new MyIncrAction();
    MyDecrAction* decr = new MyDecrAction();

    EXPECT_EQ(0, mrs->x);
    mrs = incr->apply(mrs);
    EXPECT_EQ(1, mrs->x);
    mrs = cancel(mrs);
    EXPECT_EQ(0, mrs->x);

    delete incr;
    delete decr;
    delete mrs;
}
