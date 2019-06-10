// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#include "../actions.hh"
#include "../constant.hh"

#include "test-helpers.hh"

TEST_F(ActionTest, ActionPlay_OutOfBounds)
{
    st->set_player_turn(PLAYER_1, true);

    for (position pos : {(position){-1, 1}, {3, 1}, {0, 4}, {2, -7}})
    {
        ActionPlay act(pos, PLAYER_1);
        EXPECT_EQ(OUT_OF_BOUNDS, act.check(st));
    }
}

TEST_F(ActionTest, ActionPlay_AlreadyOccupied)
{
    st->set_player_turn(PLAYER_1, true);
    st->set_player_turn(PLAYER_2, true);

    position pos = {1, 1};
    ActionPlay act(pos, PLAYER_1);
    EXPECT_EQ(OK, act.check(st));

    act.apply_on(st);
    EXPECT_EQ(PLAYER_1, st->get_cell(pos));

    ActionPlay act2(pos, PLAYER_1);
    EXPECT_EQ(ALREADY_OCCUPIED, act2.check(st));
    EXPECT_EQ(PLAYER_1, st->get_cell(pos));

    ActionPlay act3(pos, PLAYER_2);
    EXPECT_EQ(ALREADY_OCCUPIED, act3.check(st));
    EXPECT_EQ(PLAYER_1, st->get_cell(pos));
}

TEST_F(ActionTest, ActionPlay_AlreadyPlayed)
{
    st->set_player_turn(PLAYER_1, true);

    ActionPlay act({0, 0}, PLAYER_1);
    EXPECT_EQ(OK, act.check(st));
    act.apply_on(st);

    ActionPlay act2({1, 1}, PLAYER_1);
    EXPECT_EQ(ALREADY_PLAYED, act2.check(st));
}

TEST_F(ActionTest, ActionPlay_Ok)
{
    st->set_player_turn(PLAYER_1, true);
    st->set_player_turn(PLAYER_2, true);

    ActionPlay act({0, 0}, PLAYER_1);
    EXPECT_EQ(OK, act.check(st));
    EXPECT_EQ(st->NO_PLAYER, st->get_cell({0, 0}));
    act.apply_on(st);
    EXPECT_EQ(PLAYER_1, st->get_cell({0, 0}));

    ActionPlay act2({2, 2}, PLAYER_2);
    EXPECT_EQ(OK, act2.check(st));
    EXPECT_EQ(st->NO_PLAYER, st->get_cell({2, 2}));
    act2.apply_on(st);
    EXPECT_EQ(PLAYER_2, st->get_cell({2, 2}));
}
