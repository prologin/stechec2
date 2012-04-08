#ifndef LIB_RULES_PLAYER_HH_
# define LIB_RULES_PLAYER_HH_

# include <cstdint>
# include <list>

namespace rules {

class Player
{
public:
    Player(uint32_t id_, int32_t score_)
        : id(id_),
          score(score_)
    {}

    uint32_t id;
    int32_t score;
};

typedef std::list<Player> PlayerList;

} // namespace rules

#endif // LIB_RULES_PLAYER_HH_
