// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <utils/buffer.hh>
#include <vector>

namespace rules {

struct Player final
{
    Player() = default;
    Player(uint32_t id_, uint32_t type_)
        : id(id_), type(type_), score(0), nb_timeout(0), name("anonymous")
    {}

    void handle_buffer(utils::Buffer& buf)
    {
        buf.handle(id);
        buf.handle(type);
        buf.handle(score);
        /* We do not need to serialize nb_timeout. Only for the server. */
        buf.handle(name);
    }

    uint32_t id;
    uint32_t type;
    int32_t score;
    uint32_t nb_timeout;
    std::string name;
};

class Players final : public utils::IBufferizable
{
    using PlayersVector = std::vector<std::shared_ptr<Player>>;

public:
    Players() = default;
    explicit Players(PlayersVector players) : players_{std::move(players)} {}

    void handle_buffer(utils::Buffer& buf) override
    {
        if (buf.serialize())
        {
            for (const auto& player : players_)
                player->handle_buffer(buf);
        }
        else
        {
            // Reset
            players_.clear();

            while (!buf.empty())
            {
                // Get a player
                auto player = std::make_shared<Player>(0, 0);

                // And unserialize it
                player->handle_buffer(buf);

                players_.push_back(player);
            }
        }
    }

    std::string scores_yaml() const
    {
        std::stringstream ss;
        for (const auto& player : players_)
        {
            ss << "---" << std::endl
               << "player: " << player->name.c_str() << std::endl
               << "score: " << player->score << std::endl
               << "nb_timeout: " << player->nb_timeout << std::endl;
        }
        return ss.str();
    }

    void add(std::shared_ptr<Player> player) { players_.push_back(player); }

    // Sort players by identifiers
    void sort()
    {
        std::sort(players_.begin(), players_.end(),
                  [](const auto& a, const auto& b) { return a->id < b->id; });
    }

    size_t size() const { return players_.size(); }

    auto begin() const { return players_.begin(); }
    auto end() const { return players_.end(); }

    auto front() const { return players_.front(); }
    auto back() const { return players_.back(); }

    auto operator[](int i) { return players_[i]; }
    auto operator[](int i) const { return players_[i]; }

private:
    PlayersVector players_;
};

enum PlayerType
{
    PLAYER = 1,
    SPECTATOR = 2
};
const int MAX_PLAYER_TYPE = 3;

static inline std::string playertype_str(PlayerType type)
{
    switch (type)
    {
    case PLAYER:
        return "PLAYER";
    case SPECTATOR:
        return "SPECTATOR";
    }
    return {};
}

} // namespace rules
