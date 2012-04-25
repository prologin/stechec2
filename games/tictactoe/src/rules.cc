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

        sandbox_.execute(champion_init);
    }

    players_ = opt.players;
    spectators_ = opt.spectators;

    // Register Actions
    api_->actions()->register_action(0,
            []() -> rules::IAction* { return new ActionPlay(); });
}

Rules::~Rules()
{
    if (champion_)
    {
        sandbox_.execute(champion_end);
        delete champion_;
    }

    delete api_;
}

void Rules::client_loop(rules::ClientMessenger_sptr msgr)
{
    CHECK(champion_ != nullptr);

    uint32_t playing_id;

    while (true)
    {
        while (((winner_ = is_finished()) == -1) &&
                msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            if (is_spectator(playing_id))
                continue;

            // Pull actions
            api_->actions()->clear();
            msgr->pull_actions(api_->actions());

            // Apply them onto the gamestate
            for (auto& action : api_->actions()->actions())
            {
                // Only apply others actions
                if (action->player_id() == api_->player()->id)
                    continue;

                api_->game_state_set(action->apply(api_->game_state()));
            }
        }

        if (winner_ != -1)
            break;

        DEBUG("NEW TURN");

        // Play
        api_->actions()->clear();
        sandbox_.execute(champion_play);

        // Send actions
        msgr->send_actions(*api_->actions());
        msgr->wait_for_ack();

        // Drop our own actions the server sends back
        msgr->pull_actions(api_->actions());

        // XXX: debug
        std::cout << *api_->game_state();
    }

    DEBUG("winner = %i", winner_);
}

void Rules::server_loop(rules::ServerMessenger_sptr msgr)
{
    while (true)
    {
        for (auto& player : players_->players)
        {
            DEBUG("NEW TURN");

            // Send id of the player who should play
            msgr->push_id(player->id);

            // Receive actions
            api_->actions()->clear();
            msgr->recv_actions(api_->actions());

            // Apply them onto the gamestate
            for (auto& action : api_->actions()->actions())
                api_->game_state_set(action->apply(api_->game_state()));

            msgr->ack();

            // Send actions
            msgr->push_actions(*api_->actions());

            // XXX: debug
            std::cout << *api_->game_state();

            if ((winner_ = is_finished()) != -1)
                break;
        }

        if ((winner_ = is_finished()) != -1)
            break;
    }

    DEBUG("winner = %i", winner_);
}

void Rules::spectator_loop(rules::ClientMessenger_sptr msgr)
{
    CHECK(champion_ != nullptr);

    uint32_t playing_id;

    while (true)
    {
        while (((winner_ = is_finished()) == -1) &&
                msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            if (is_spectator(playing_id))
                continue;

            // Pull actions
            api_->actions()->clear();
            msgr->pull_actions(api_->actions());

            // Apply them onto the gamestate
            for (auto& action : api_->actions()->actions())
            {
                // Only apply others actions
                if (action->player_id() == api_->player()->id)
                    continue;

                api_->game_state_set(action->apply(api_->game_state()));
            }
        }

        if (winner_ != -1)
            break;

        DEBUG("NEW TURN");

        // Spectate
        champion_play();

        // Notify the server we are done
        msgr->ack();
        msgr->wait_for_ack();

        // XXX: debug
        std::cout << *api_->game_state();
    }

    DEBUG("winner = %i", winner_);
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

bool Rules::is_spectator(uint32_t id)
{
    for (rules::Player_sptr spectator : spectators_->players)
        if (spectator->id == id)
            return true;

    return false;
}
