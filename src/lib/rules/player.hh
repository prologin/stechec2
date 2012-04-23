#ifndef LIB_RULES_PLAYER_HH_
# define LIB_RULES_PLAYER_HH_

# include <cstdint>
# include <memory>
# include <list>
# include <string>
# include <utils/buffer.hh>

namespace rules {

struct Player
{
    Player(uint32_t id_, uint32_t type_)
        : id(id_),
          type(type_),
          score(0)
    {}

    void handle_buffer(utils::Buffer& buf)
    {
        buf.handle(id);
        buf.handle(type);
        buf.handle(score);
    }

    uint32_t id;
    uint32_t type;
    int32_t score;
};

typedef std::shared_ptr<Player> Player_sptr;
typedef std::list<Player_sptr> PlayerList;

enum PlayerType
{
    PLAYER                      = 1,
    SPECTATOR                   = 2,
    NONBLOCKING_SPECTATOR       = 3
};

static inline std::string playertype_str(PlayerType type)
{
    switch (type)
    {
    case PLAYER:
        return "PLAYER";
    case SPECTATOR:
        return "SPECTATOR";
    case NONBLOCKING_SPECTATOR:
        return "NONBLOCKING SPECTATOR";
    }

    return "";
}

} // namespace rules

#endif // LIB_RULES_PLAYER_HH_
