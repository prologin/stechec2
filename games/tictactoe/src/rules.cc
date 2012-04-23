#include "rules.hh"

#include <utils/log.hh>
#include <utils/buffer.hh>
#include <iostream>

#include "action-play.hh"
#include "game-state.hh"

Rules::Rules(const rules::Options& opt)
    : opt_(opt),
      champion_(nullptr)
{
    GameState* game_state = new GameState();
    api_ = new Api(game_state, opt.player);

    // Get the champion library if we are a client
    if (!opt.champion_lib.empty())
    {
        champion_ = new utils::DLL(opt.champion_lib);

        champion_init = champion_->get<f_champion_init>("init_game");
        champion_play = champion_->get<f_champion_play>("play_turn");
        champion_end = champion_->get<f_champion_end>("end_game");

        champion_init();
    }

    players_ = opt.players;

    // Register Actions
    api_->actions()->register_action(0,
            []() -> rules::IAction* { return new ActionPlay(); });
}

Rules::~Rules()
{
    if (champion_)
    {
        champion_end();
        delete champion_;
    }

    delete api_;
}

void Rules::client_loop(net::ClientMessenger_sptr msgr)
{
    CHECK(champion_ != nullptr);

    while ((winner_ = is_finished()) == -1)
    {
        std::cout << *api_->game_state();

        // Play
        champion_play();

        // Send actions
        utils::Buffer send_buf;

        for (auto& action : api_->actions()->actions())
            action->handle_buffer(send_buf);

        msgr->send(send_buf);
        msgr->wait_for_ack();

        for (uint32_t i = 0; i < players_->players.size(); ++i)
        {
            // Receive actions
            utils::Buffer* pull_buf = msgr->pull();

            // Put them in the API container
            api_->actions()->handle_buffer(*pull_buf);

            // Apply them onto the gamestate
            for (auto& action : api_->actions()->actions())
                api_->game_state_set(action->apply(api_->game_state()));
        }
    }

    // Send actions
    utils::Buffer send_buf;

    for (auto& action : api_->actions()->actions())
        action->handle_buffer(send_buf);

    msgr->send(send_buf);
    msgr->wait_for_ack();
}

void Rules::server_loop(net::ServerMessenger_sptr msgr)
{
    rules::IActionList action_list;

    while ((winner_ = is_finished()) == -1)
    {
        std::cout << *api_->game_state();

        DEBUG("winner = %i", winner_);

        for (uint32_t i = 0; i < players_->players.size(); ++i)
        {
            // Receive actions
            utils::Buffer* pull_buf = msgr->recv();

            // Put them in the API container
            api_->actions()->handle_buffer(*pull_buf);

            // Apply them onto the gamestate
            for (auto& action : api_->actions()->actions())
            {
                api_->game_state_set(action->apply(api_->game_state()));
                action_list.push_back(action);
            }

            msgr->ack();
        }

        // Send actions
        utils::Buffer send_buf;

        for (auto& action : action_list)
            action->handle_buffer(send_buf);

        msgr->push(send_buf);
    }
}

int Rules::is_finished()
{
    const std::vector<int> board = api_->game_state()->board();

    // Lines
    for (int i = 0; i < 9; i += 3)
    {
        if (board[i] != -1 &&
                board[i] == board[i + 1] && board[i + 1] == board[i + 6])
            return board[i];
    }

    // Columns
    for (int i = 0; i < 3; ++i)
    {
        if (board[i] != -1 &&
                board[i] == board[i + 3] && board[i + 3] == board[i + 6])
            return board[i];
    }

    // Diagonals
    if (board[0] != -1 && board[0] == board[4] && board[4] == board[8])
        return board[0];

    if (board[2] != -1 && board[2] == board[4] && board[4] == board[6])
        return board[2];

    return -1;
}
