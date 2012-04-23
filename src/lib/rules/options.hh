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

    // Players
    PlayerVector_sptr players;

    // Verbosity of the logs displayed/written
    unsigned verbose;
};

} // namespace rules

#endif // !LIB_RULES_OPTIONS_HH
