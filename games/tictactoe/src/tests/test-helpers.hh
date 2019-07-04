// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2018 Association Prologin <association@prologin.org>
#pragma once

#include <memory>

#include <gtest/gtest.h>

#include "../api.hh"
#include "../game_state.hh"
#include "../rules.hh"

static rules::Players make_players(int id1, int id2)
{
    // Create two players (no spectator)
    rules::Players players;
    players.add(std::make_shared<rules::Player>(id1, rules::PLAYER));
    players.add(std::make_shared<rules::Player>(id2, rules::PLAYER));
    return players;
}

class ActionTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        st.reset(new GameState(make_players(PLAYER_1, PLAYER_2)));
    }

    std::unique_ptr<GameState> st;

    const int PLAYER_1 = 0;
    const int PLAYER_2 = 1;
};

class ApiTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        auto rules_players = make_players(PLAYER_1, PLAYER_2);
        players[0].id = PLAYER_1;
        players[0].api = std::make_unique<Api>(
            std::make_unique<GameState>(rules_players), rules_players.all()[0]);
        players[1].id = PLAYER_2;
        players[1].api = std::make_unique<Api>(
            std::make_unique<GameState>(rules_players), rules_players.all()[1]);
    }

    struct Player
    {
        int id;
        std::unique_ptr<Api> api;
    };
    std::array<Player, 2> players;

    // Players values are not 0 and 1, because that would be too simple
    const int PLAYER_1 = 1337;
    const int PLAYER_2 = 42;
};

class RulesTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        rules::Options opt;
        opt.map_file = "";
        opt.players = make_players(PLAYER_1, PLAYER_2);
        rules.reset(new Rules(opt));
    }

    std::unique_ptr<Rules> rules;

    const int PLAYER_1 = 3;
    const int PLAYER_2 = 7;
};
