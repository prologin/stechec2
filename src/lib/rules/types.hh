#ifndef LIB_RULES_TYPES_HH_
#define LIB_RULES_TYPES_HH_

#include <rules/client-messenger.hh>
#include <rules/options.hh>
#include <rules/server-messenger.hh>

namespace rules {

typedef void (*f_rules_init)(const Options&);
typedef void (*f_rules_result)();

typedef void (*f_client_loop)(ClientMessenger_sptr);
typedef void (*f_server_loop)(ServerMessenger_sptr);

} // namespace rules

#endif // !LIB_RULES_TYPES_HH_
