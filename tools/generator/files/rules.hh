#ifndef RULES_RULES_HH_
#define RULES_RULES_HH_

#include <utils/dll.hh>
#include <utils/sandbox.hh>
#include <rules/options.hh>
#include <rules/client-messenger.hh>
#include <rules/server-messenger.hh>
#include <rules/player.hh>
#include <rules/rules.hh>
#include <rules/actions.hh>

#include "api.hh"

class Rules : // FIXME inheritance
{
public:
    explicit Rules(const rules::Options opt);
    virtual ~Rules();

    // FIXME
    // If you inherit from TurnBasedRules or SynchronousRules,
    // keep 1) and delete 2) ; else, keep 2) and delete 1)

    // 1)
    virtual rules::Actions* get_actions();
    virtual void apply_action(const rules::IAction_sptr& action);
    virtual bool is_finished();

    // 2)
    virtual void player_loop(rules::ClientMessenger_sptr msgr);
    virtual void spectator_loop(rules::ClientMessenger_sptr msgr);
    virtual void server_loop(rules::ServerMessenger_sptr msgr);


protected:
    // FIXME
    // include user functions here
    
private:
    utils::DLL* champion_dll_;
    Api* api_;
    utils::Sandbox sandbox_;
    // FIXME
};

#endif // !RULES_RULES_HH_
