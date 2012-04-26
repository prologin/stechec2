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

    // Map file name
    std::string map_name;

    // Memory limit for the champion in MB
    uint32_t memory;

    // Turn time limit for the champion in ms
    uint32_t time;

    // Player
    Player_sptr player;

    // Players
    Players_sptr players;

    // Spactators
    Players_sptr spectators;

    // Verbosity of the logs displayed/written
    unsigned verbose;
};

} // namespace rules

#endif // !LIB_RULES_OPTIONS_HH
