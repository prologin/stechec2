#ifndef LIB_RULES_PLAYER_HH_
# define LIB_RULES_PLAYER_HH_

# include <cstdint>
# include <memory>
# include <vector>
# include <string>
# include <utils/buffer.hh>

namespace rules {

struct Player
{
    Player(uint32_t id_, uint32_t type_)
        : id(id_),
          type(type_),
          score(0),
          name("anonymous")
    {}

    void handle_buffer(utils::Buffer& buf)
    {
        buf.handle(id);
        buf.handle(type);
        buf.handle(score);
        buf.handle(name);
    }

    uint32_t id;
    uint32_t type;
    int32_t score;
    std::string name;
};

typedef std::shared_ptr<Player> Player_sptr;

struct Players
{
    void handle_buffer(utils::Buffer& buf)
    {
        if (buf.serialize())
        {
            for (unsigned i = 0; i < players.size(); ++i)
                players[i]->handle_buffer(buf);
        }
        else
        {
            while (!buf.empty())
            {
                // Get a player
                Player_sptr player = Player_sptr(new Player(0, 0));

                // And unserialize it
                player->handle_buffer(buf);
                players.push_back(player);
            }
        }
    }

    size_t size() const
    {
        return players.size();
    }

    std::vector<Player_sptr> players;
};

typedef std::shared_ptr<Players> Players_sptr;

enum PlayerType
{
    PLAYER                      = 1,
    SPECTATOR                   = 2
};

static inline std::string playertype_str(PlayerType type)
{
    switch (type)
    {
    case PLAYER:
        return "PLAYER";
    case SPECTATOR:
        return "SPECTATOR";
    }

    return "";
}

} // namespace rules

#endif // LIB_RULES_PLAYER_HH_
