#ifndef RULES_RULES_HH_
# define RULES_RULES_HH_

# include <utils/dll.hh>
# include <utils/sandbox.hh>
# include <rules/options.hh>
# include <rules/client-messenger.hh>
# include <rules/server-messenger.hh>
# include <rules/player.hh>


class Rules
{
public:
    explicit Rules(const rules::Options opt);
    virtual ~Rules();

    // FIXME
    void client_loop(rules::ClientMessenger_sptr msgr);
    void spectator_loop(rules::ClientMessenger_sptr msgr);
    void server_loop(rules::ServerMessenger_sptr msgr);

private:
    utils::DLL* champion_dll_;

    // FIXME
};

#endif // !RULES_RULES_HH_
