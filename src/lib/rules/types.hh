#ifndef TYPES_HH_
# define TYPES_HH_

# include "player.hh"
# include "action.hh"

namespace rules {

struct PlayerActions
{
    Player_sptr player;
    IActionList actions;
};

typedef std::list<PlayerActions> PlayerActionsList;

typedef void (*f_rules_init)();
typedef bool (*f_rules_turn)(PlayerActionsList* in, IActionList* out);
typedef void (*f_rules_result)(PlayerList*);

} // namespace rules

#endif // !TYPES_HH_
