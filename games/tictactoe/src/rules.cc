#include "rules.hh"

#include "game-state.hh"

Rules::Rules(const rules::Options& opt)
    : opt_(opt),
      champion_(nullptr)
{
    GameState* game_state = new GameState();
    api_ = new Api(game_state, opt.player);

    if (!opt.champion_lib.empty())
    {
        champion_ = new utils::DLL(opt.champion_lib);

        champion_init = champion_->get<f_champion_init>("init_game");
        champion_play = champion_->get<f_champion_play>("play_turn");
        champion_end = champion_->get<f_champion_end>("end_game");
    }

    champion_init();
}

Rules::~Rules()
{
    delete champion_;
    delete api_;
}

void Rules::client_loop(net::ClientMessenger_sptr msgr)
{
}

void Rules::server_loop(net::ServerMessenger_sptr msgr)
{
}
