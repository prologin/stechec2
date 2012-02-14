#ifndef LIB_NET_COMMON_HH_
# define LIB_NET_COMMON_HH_

# include <string>

namespace net {

enum ClientType
{
    PLAYER                      = 1,
    SPECTATOR                   = 2,
    NONBLOCKING_SPECTATOR       = 3
};

static inline std::string clienttype_str(ClientType type)
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
}

} // namespace net

#endif // !LIB_NET_COMMON_HH_
