#ifndef LIB_RULES_OPTIONS_HH
# define LIB_RULES_OPTIONS_HH

# include <string>
# include <cstdint>

# include "player.hh"

namespace rules {

struct Options
{
    // Path to the champion library
    std::string champion_lib;

    // Memory limit for the champion
    uint32_t memory;

    // Turn time limit for the champion
    uint32_t time;

    // Player
    Player_sptr player;
};

} // namespace rules

#endif // !LIB_RULES_OPTIONS_HH
