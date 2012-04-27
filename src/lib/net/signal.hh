#ifndef SIGNAL_HH_
# define SIGNAL_HH_

# include <string>

namespace net {

enum signal_type
{
    CLIENT_SIGNAL,
    SERVER_SIGNAL
};

void handle_signals(signal_type, const std::string& addr, int nbr);

} // namespace net

#endif // !SIGNAL_HH_
