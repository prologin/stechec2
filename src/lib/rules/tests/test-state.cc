#include "../game-state.hh"

#include <gtest/gtest.h>

using namespace rules;

class MockGameState : public GameState
{
    virtual GameState* copy() const { return new MockGameState(); }
};

// Check get_old_version / set_old_version
TEST(GameState, OldVersion)
{
    MockGameState* s1 = new MockGameState();
    MockGameState* s2 = new MockGameState();

    // Check if the old version is correctly initialized to NULL
    EXPECT_EQ(0, s1->get_old_version());

    // Check if set_old_version works
    s1->set_old_version(s2);
    EXPECT_EQ(s2, s1->get_old_version());

    delete s1;
}

class ChainDeleteGameState : public MockGameState
{
public:
    ChainDeleteGameState(bool* flag) : MockGameState(), flag_(flag) {}
    virtual ~ChainDeleteGameState() { *flag_ = true; }

private:
    bool* flag_;
};

// Check if deleting the new version also deletes older versions
TEST(GameState, ChainDelete)
{
    bool s1_deleted = false;
    bool s2_deleted = false;

    ChainDeleteGameState* s1 = new ChainDeleteGameState(&s1_deleted);
    ChainDeleteGameState* s2 = new ChainDeleteGameState(&s2_deleted);

    s1->set_old_version(s2);
    delete s1;

    EXPECT_TRUE(s1_deleted);
    EXPECT_TRUE(s2_deleted);
}
