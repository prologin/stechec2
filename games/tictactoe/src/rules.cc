#include "rules.hh"

#include <utils/log.hh>
#include <utils/buffer.hh>

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
        // Receive actions
        utils::Buffer* pull_buf = msgr->pull();

        // Apply actions

        // Play
        champion_play();

        // Send actions
        utils::Buffer send_buf;

        //for (const auto& action : api_->actions())
        //    action.handle_buffer(send_buf);

        msgr->send(send_buf);
    }
}

void Rules::server_loop(net::ServerMessenger_sptr msgr)
{
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
