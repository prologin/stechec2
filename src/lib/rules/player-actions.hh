#ifndef PLAYER_ACTIONS_HH_
# define PLAYER_ACTIONS_HH_

# include <cstdint>
# include <list>
# include <unordered_map>
# include <utils/buffer.hh>

# include "action.hh"

namespace rules {

class PlayerActions
{
public:
    // To handle unserialization of multiple Actions, we have to be able to
    // instantiate the corresponding objects
    void register_action(uint32_t action_id, f_action_factory action_factory);

    // Serialization/Unserialization
    void handle_buffer(utils::Buffer& buf);

private:
    Player_sptr player_;
    IActionList actions_;
    std::unordered_map<uint32_t, f_action_factory> action_factory_;
};

typedef std::list<PlayerActions> PlayerActionsList;

} // namespace rules

#endif // !PLAYER_ACTIONS_HH_
