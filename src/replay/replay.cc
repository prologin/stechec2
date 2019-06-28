// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "replay.hh"

#include <fstream>
#include <iterator>

#include <gflags/gflags.h>

#include <rules/replay-messenger.hh>
#include <utils/log.hh>

DEFINE_string(rules, "", "Rules library");
DEFINE_string(replay, "", "Replay file");

bool Replay::check() const
{
    if (FLAGS_rules.empty())
    {
        ERR("--rules cannot be empty");
        return false;
    }
    if (FLAGS_replay.empty())
    {
        ERR("--replay cannot be empty");
        return false;
    }
    return true;
}

void Replay::run()
{
    // Get required functions from the rules library
    auto rules_lib = std::make_unique<utils::DLL>(FLAGS_rules);
    auto rules_init = rules_lib->get<rules::f_rules_init>("rules_init");
    auto replay_loop = rules_lib->get<rules::f_replay_loop>("replay_loop");
    auto rules_result = rules_lib->get<rules::f_rules_result>("rules_result");

    auto replay = read_replay(FLAGS_replay);
    DEBUG("Read %d bytes from %s", replay.size(), FLAGS_replay.c_str());

    auto map_content = read_map(&replay);
    if (map_content.empty())
        DEBUG("No map in replay");
    else
        DEBUG("Read map of %d bytes", map_content.size());

    auto players = read_players(&replay);
    DEBUG("Read %d players from replay", players->size());

    auto spectators = read_players(&replay);
    if (spectators->size() == 0)
        DEBUG("No spectator in replay");
    else
        DEBUG("Read %d spectators from replay", spectators->size());

    // Set the rules options
    rules::Options rules_opt{};
    rules_opt.map_content = map_content;
    rules_opt.verbose = FLAGS_verbose;
    rules_opt.players = players;
    rules_opt.spectators = spectators;
    rules_init(rules_opt);

    // Process actions from replay
    auto msgr = std::make_shared<rules::ReplayMessenger>(&replay);
    replay_loop(msgr);

    rules_result();

    auto replay_result = read_result(&replay);

    if (!replay.empty())
        WARN("Replay contains %d extra bytes", replay.size());

    // Compare saved and replayed results
    if (!compare_results(*replay_result, *players))
    {
        WARN("Match results mismatch!");
        WARN("Results saved in replay:");
        std::cout << replay_result->scores_yaml();
        WARN("Computed results:");
    }

    std::cout << players->scores_yaml();
}

utils::Buffer Replay::read_replay(const std::string& replay_path)
{
    auto stream = std::ifstream(replay_path, std::ios::binary);
    std::vector<uint8_t> replay_data;
    std::for_each(std::istreambuf_iterator<char>(stream),
                  std::istreambuf_iterator<char>(),
                  [&replay_data](const char c) { replay_data.push_back(c); });
    return utils::Buffer{replay_data};
}

std::string Replay::read_map(utils::Buffer* replay)
{
    std::string map;
    replay->handle(map);
    return map;
}

rules::Players_sptr Replay::read_players(utils::Buffer* replay)
{
    auto players = std::make_shared<rules::Players>();
    replay->handle_bufferizable(players.get());
    return players;
}

rules::Players_sptr Replay::read_result(utils::Buffer* replay)
{
    // Final scores are extracted from the players save
    return read_players(replay);
}

bool Replay::compare_results(const rules::Players& ref,
                             const rules::Players& actual) const
{
    if (ref.size() != actual.size())
        return false;
    for (size_t i = 0; i < ref.size(); ++i)
    {
        const auto& player_ref = ref.players[i];
        const auto& player_actual = actual.players[i];
        if (player_ref->id != player_actual->id)
            return false;
        if (player_ref->name != player_actual->name)
            return false;
        if (player_ref->score != player_actual->score)
            return false;
    }
    return true;
}
