#include <vector>

#include <gtest/gtest.h>

#include "../constant.hh"
#include "../game-state.hh"

#include "../action-play.hh"


class GameStateTest : public ::testing::Test
{
};

typedef std::pair<int, int> coordinates_t;

// Test that GameState::winner works fine
TEST_F(GameStateTest, Winner)
{
    // The code coverage for this on GameState::winner will be quite bad
    // because this test is more than incomplete.
    GameState st;

    EXPECT_EQ(st.NO_PLAYER, st.winner());
}
