#include "../state.hh"

#include <gtest/gtest.h>

class MockRulesState : public RulesState
{
    virtual RulesState* copy() const { return new MockRulesState(); }
};

// Check get_old_version / set_old_version
TEST(RulesState, OldVersion)
{
    MockRulesState* s1 = new MockRulesState();
    MockRulesState* s2 = new MockRulesState();

    // Check if the old version is correctly initialized to NULL
    EXPECT_EQ(0, s1->get_old_version());

    // Check if set_old_version works
    s1->set_old_version(s2);
    EXPECT_EQ(s2, s1->get_old_version());

    delete s1;
}

class ChainDeleteRulesState : public MockRulesState
{
public:
    ChainDeleteRulesState(bool* flag) : MockRulesState(), flag_(flag) {}
    virtual ~ChainDeleteRulesState() { *flag_ = true; }

private:
    bool* flag_;
};

// Check if deleting the new version also deletes older versions
TEST(RulesState, ChainDelete)
{
    bool s1_deleted = false;
    bool s2_deleted = false;

    ChainDeleteRulesState* s1 = new ChainDeleteRulesState(&s1_deleted);
    ChainDeleteRulesState* s2 = new ChainDeleteRulesState(&s2_deleted);

    s1->set_old_version(s2);
    delete s1;

    EXPECT_TRUE(s1_deleted);
    EXPECT_TRUE(s2_deleted);
}
