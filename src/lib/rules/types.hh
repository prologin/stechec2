#ifndef LIB_RULES_TYPES_HH_
# define LIB_RULES_TYPES_HH_

# include <rules/player.hh>
# include <rules/action.hh>
# include <net/client-messenger.hh>
# include <net/server-messenger.hh>

namespace rules {

typedef std::list<PlayerActions> PlayerActionsList;

typedef void (*f_rules_init)();
typedef void (*f_rules_result)(PlayerList*);

typedef void (*f_client_loop)(::net::ClientMessenger_sptr);
typedef void (*f_server_loop)(::net::ServerMessenger_sptr);

} // namespace rules

#endif // !LIB_RULES_TYPES_HH_
