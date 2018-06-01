#ifndef RULES_RULES_HH_
#define RULES_RULES_HH_

#include <memory>

#include <rules/actions.hh>
#include <rules/client-messenger.hh>
#include <rules/options.hh>
#include <rules/player.hh>
#include <rules/rules.hh>
#include <rules/server-messenger.hh>
#include <utils/dll.hh>
#include <utils/sandbox.hh>

#include "api.hh"

typedef void (*f_champion_init_game)();
typedef void (*f_champion_play_turn)();
typedef void (*f_champion_end_game)();

class Rules : public rules::TurnBasedRules
{
public:
    explicit Rules(const rules::Options opt);
    virtual ~Rules() {}

    rules::Actions* get_actions() override;
    void apply_action(const rules::IAction_sptr& action) override;
    bool is_finished() override;

    void at_player_start(rules::ClientMessenger_sptr) override;
    void at_spectator_start(rules::ClientMessenger_sptr) override;
    void at_player_end(rules::ClientMessenger_sptr) override;
    void at_spectator_end(rules::ClientMessenger_sptr) override;

    void player_turn() override;
    void spectator_turn() override;

    void start_of_player_turn(unsigned int player_id) override;
    void end_of_player_turn(unsigned int player_id) override;

protected:
    f_champion_init_game champion_init_game_;
    f_champion_play_turn champion_play_turn_;
    f_champion_end_game champion_end_game_;

private:
    void register_actions();

    std::unique_ptr<utils::DLL> champion_dll_;
    std::unique_ptr<Api> api_;
    utils::Sandbox sandbox_;
};

#endif // !RULES_RULES_HH_
