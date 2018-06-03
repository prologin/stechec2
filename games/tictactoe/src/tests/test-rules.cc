#include "test-helpers.hh"

TEST_F(RulesTest, Rules_FinishWin)
{
    int cnt = 0;
    while (!rules->is_finished())
    {
        rules->start_of_player_turn(PLAYER_1);
        rules->end_of_player_turn(PLAYER_1);

        EXPECT_FALSE(rules->is_finished());

        rules->start_of_player_turn(PLAYER_2);
        rules->get_game_state()->set_cell({0, cnt}, PLAYER_2);
        rules->end_of_player_turn(PLAYER_2);

        cnt++;
        if (cnt == 3)
            EXPECT_TRUE(rules->is_finished());
        else
            EXPECT_FALSE(rules->is_finished());
    }
}

TEST_F(RulesTest, Rules_FinishDraw)
{
    std::vector<position> to_play1 = {{0, 1}, {1, 0}, {1, 2}, {2, 0}, {2, 2}};
    std::vector<position> to_play2 = {{0, 0}, {0, 2}, {1, 1}, {2, 1}};
    size_t idx1 = 0;
    size_t idx2 = 0;
    while (!rules->is_finished())
    {
        rules->start_of_player_turn(PLAYER_1);
        rules->get_game_state()->set_cell(to_play1[idx1++], PLAYER_1);
        rules->end_of_player_turn(PLAYER_1);

        if (idx1 == to_play1.size())
            EXPECT_TRUE(rules->is_finished());
        else
            EXPECT_FALSE(rules->is_finished());

        rules->start_of_player_turn(PLAYER_2);
        if (idx2 < to_play2.size())
            rules->get_game_state()->set_cell(to_play2[idx2++], PLAYER_2);
        rules->end_of_player_turn(PLAYER_2);

        if (idx1 == to_play1.size())
            EXPECT_TRUE(rules->is_finished());
        else
            EXPECT_FALSE(rules->is_finished());
    }
}
