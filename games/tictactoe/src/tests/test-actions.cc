#include <vector>

#include <gtest/gtest.h>

#include "../constant.hh"
#include "../game-state.hh"

#include "../action-play.hh"


class ActionsTest : public ::testing::Test
{
    protected:
        virtual void SetUp() override
        {
            utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
            gamestate_ = new GameState();
        }

        virtual void TearDown() override
        {
            if (gamestate_ != nullptr)
                delete gamestate_;
            gamestate_ = nullptr;
        }

        const int PLAYER_1 = 1;
        const int PLAYER_2 = 2;

        GameState* gamestate_;
};


typedef std::pair<int, int> coordinates_t;


// Test that ActionPlay::check correctly detects out of bound coordinates
TEST_F(ActionsTest, Check_OutOfBounds)
{
    std::vector<coordinates_t> coordinates = {
        coordinates_t(-1, 0),
        coordinates_t(3, 0),
        coordinates_t(0, -1),
        coordinates_t(0, 3)
    };

    for (auto coord : coordinates)
    {
        ActionPlay action(coord.first, coord.second, PLAYER_1);
        EXPECT_EQ(OUT_OF_BOUNDS, action.check(gamestate_));
    }
}

// Test that ActionPlay::check correctly detects already occupied cells
TEST_F(ActionsTest, Check_AlreadyCorrect)
{
    ActionPlay action1(0, 0, PLAYER_1);
    ActionPlay action2(0, 0, PLAYER_2);

    // First occupy the cell
    EXPECT_EQ(OK, action1.check(gamestate_));
    GameState* next_state = action1.apply(gamestate_);
    EXPECT_EQ(PLAYER_1, next_state->cell(0, 0));

    // Then check that no player can use the cell again, and that the cell
    // stays unaffected.
    EXPECT_EQ(ALREADY_OCCUPIED, action1.check(next_state));
    EXPECT_EQ(PLAYER_1, next_state->cell(0, 0));

    EXPECT_EQ(ALREADY_OCCUPIED, action2.check(next_state));
    EXPECT_EQ(PLAYER_1, next_state->cell(0, 0));

    /* This will destroy the old state: clear it afterwards so that we don't
       attempt to double-free it.  */
    delete next_state;
    gamestate_ = nullptr;
}

// Test that ActionPlay::apply_on correctly changes its target cell
TEST_F(ActionsTest, ApplyOn)
{
    ActionPlay action1(1, 2, PLAYER_1);

    EXPECT_EQ(gamestate_->NO_PLAYER, gamestate_->cell(1, 2));
    EXPECT_EQ(OK, action1.check(gamestate_));

    GameState* next_state = action1.apply(gamestate_);
    EXPECT_EQ(PLAYER_1, next_state->cell(1, 2));

    /* See the above comment.  */
    delete next_state;
    gamestate_ = nullptr;
}
