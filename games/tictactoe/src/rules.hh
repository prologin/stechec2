#ifndef RULES_RULES_HH_
# define RULES_RULES_HH_

# include <rules/client-messenger.hh>
# include <rules/options.hh>
# include <rules/server-messenger.hh>
# include <utils/dll.hh>
# include <utils/sandbox.hh>

# include "api.hh"

typedef void (*f_champion_init)();
typedef void (*f_champion_play)();
typedef void (*f_champion_end)();

class Rules
{
public:
    explicit Rules(const rules::Options& opt);
    virtual ~Rules();

    void client_loop(rules::ClientMessenger_sptr msgr);
    void server_loop(rules::ServerMessenger_sptr msgr);
    void spectator_loop(rules::ClientMessenger_sptr msgr);

protected:
    int is_finished();
    bool is_spectator(uint32_t id);
    void timeout_player(rules::Player_sptr);
    void erase_timeout_players();

protected:
    f_champion_init champion_init;
    f_champion_play champion_play;
    f_champion_end champion_end;

private:
    const rules::Options& opt_;

    utils::DLL* champion_;
    utils::Sandbox sandbox_;

    Api* api_;
    rules::Players_sptr players_;
    rules::Players_sptr spectators_;
    rules::Players_sptr timeout_players_;
    rules::Players_sptr to_erase_players_;
    int winner_;
};

#endif // !RULES_RULES_HH_
