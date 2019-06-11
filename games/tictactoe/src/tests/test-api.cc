// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#include "../api.hh"

#include "test-helpers.hh"

TEST_F(ApiTest, Api_MyTeam)
{
    for (auto& player : players)
        EXPECT_EQ(player.id, player.api->my_team());
}

TEST_F(ApiTest, Api_Board)
{
    for (auto& player : players)
    {
        std::vector<int> board = player.api->board();
        std::vector<int> expected(9, player.api->game_state()->NO_PLAYER);
        EXPECT_EQ(board, expected);

        player.api->game_state()->set_player_turn(player.id, true);
        if (player.id == PLAYER_1)
        {
            EXPECT_EQ(OK, player.api->play({0, 0}));
            board = player.api->board();
            EXPECT_EQ(board[0], PLAYER_1);
        }
        else
        {
            EXPECT_EQ(OK, player.api->play({1, 1}));
            board = player.api->board();
            EXPECT_EQ(board[4], PLAYER_2);
        }
    }
}

TEST_F(ApiTest, Api_Cancel)
{
    for (auto& player : players)
    {
        EXPECT_FALSE(player.api->cancel());
        player.api->game_state()->set_player_turn(player.id, true);

        EXPECT_EQ(OK, player.api->play({0, 0}));
        EXPECT_EQ(player.id, player.api->board()[0]);

        EXPECT_TRUE(player.api->cancel());
        EXPECT_EQ(player.api->game_state()->NO_PLAYER, player.api->board()[0]);
    }
}
