#ifndef TEST_HELPERS_HH
#define TEST_HELPERS_HH

#include <gtest/gtest.h>

#include "../api.hh"
#include "../game_state.hh"
#include "../rules.hh"

static rules::Players_sptr make_players(int id1, int id2)
{
    // Create two players (no spectator)
    return rules::Players_sptr(
        new rules::Players{std::vector<rules::Player_sptr>{
            rules::Player_sptr(new rules::Player(id1, rules::PLAYER)),
            rules::Player_sptr(new rules::Player(id2, rules::PLAYER)),
        }});
}

class ActionTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        st = new GameState(make_players(PLAYER_1, PLAYER_2));
    }

    virtual void TearDown() { delete st; }

    GameState* st;

    const int PLAYER_1 = 0;
    const int PLAYER_2 = 1;
};

class ApiTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        auto players_ptr = make_players(PLAYER_1, PLAYER_2);
        players[0].id = PLAYER_1;
        players[0].api = new Api(new GameState(players_ptr), players_ptr->players[0]);
        players[1].id = PLAYER_2;
        players[1].api = new Api(new GameState(players_ptr), players_ptr->players[1]);
    }

    virtual void TearDown()
    {
        delete players[0].api->game_state();
        delete players[0].api;
        delete players[1].api->game_state();
        delete players[1].api;
    }

    struct Player
    {
        int id;
        Api* api;
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

#endif
