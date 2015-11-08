#ifndef RULES_RULES_HH_
#define RULES_RULES_HH_

#include <memory>

#include <utils/dll.hh>
#include <utils/sandbox.hh>
#include <rules/options.hh>
#include <rules/client-messenger.hh>
#include <rules/server-messenger.hh>
#include <rules/player.hh>
#include <rules/rules.hh>
#include <rules/actions.hh>

#include "api.hh"

class Rules!!rules_hh_inheritance!!
{
public:
    explicit Rules(const rules::Options opt);
    virtual ~Rules();

    // @@GEN_HERE@@

private:
    void register_actions();

    std::unique_ptr<utils::DLL> champion_dll_;
    std::unique_ptr<Api> api_;
    utils::Sandbox sandbox_;
};

#endif // !RULES_RULES_HH_
