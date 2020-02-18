// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2018 Association Prologin <association@prologin.org>
#include "game_state.hh"

#include <algorithm>

GameState::GameState(const rules::Players& players)
    : rules::GameState(players)
    , board_({NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER,
              NO_PLAYER, NO_PLAYER, NO_PLAYER})
{
}

GameState* GameState::copy() const
{
    return new GameState(*this);
}

std::vector<int> GameState::get_board() const
{
    return board_;
}

bool GameState::is_valid_cell(position pos) const
{
    return 0 <= pos.x && pos.x < 3 && 0 <= pos.y && pos.y < 3;
}

int GameState::get_cell(position pos) const
{
    if (!is_valid_cell(pos))
        return NO_PLAYER;
    return board_[3 * pos.y + pos.x];
}

void GameState::set_cell(position pos, int player_id)
{
    if (is_valid_cell(pos))
        board_[3 * pos.y + pos.x] = player_id;
}

int GameState::winner() const
{
    // Check rows
    for (int row = 0; row < 3; row++)
    {
        if (get_cell({0, row}) != NO_PLAYER &&
            get_cell({0, row}) == get_cell({1, row}) &&
            get_cell({0, row}) == get_cell({2, row}))
            return get_cell({0, row});
    }

    // Check columns
    for (int col = 0; col < 3; col++)
    {
        if (get_cell({col, 0}) != NO_PLAYER &&
            get_cell({col, 0}) == get_cell({col, 1}) &&
            get_cell({col, 0}) == get_cell({col, 2}))
            return get_cell({col, 0});
    }

    // Check diagonals
    if (get_cell({0, 0}) != NO_PLAYER && get_cell({0, 0}) == get_cell({1, 1}) &&
        get_cell({0, 0}) == get_cell({2, 2}))
        return get_cell({0, 0});

    if (get_cell({0, 2}) != NO_PLAYER && get_cell({0, 2}) == get_cell({1, 1}) &&
        get_cell({0, 2}) == get_cell({2, 0}))
        return get_cell({0, 2});

    return NO_PLAYER;
}

bool GameState::is_board_full() const
{
    for (int cell : get_board())
        if (cell == NO_PLAYER)
            return false;
    return true;
}

bool GameState::is_finished() const
{
    return current_player_ == LAST_PLAYER &&
           (is_board_full() || winner() != NO_PLAYER);
}

void GameState::compute_scores()
{
    auto winner_id = winner();
    if (winner_id == NO_PLAYER)
        return;
    for (const auto& player : players_)
    {
        if (player->id == (uint32_t)winner_id)
        {
            player->score = 1;
            break;
        }
    }
}

void GameState::set_current_player(int player_id)
{
    current_player_ = player_id;
}

int GameState::get_current_player() const
{
    return current_player_;
}

void GameState::set_player_can_play(int player_id, bool can_play)
{
    player_can_play_.at(player_id) = can_play;
}
bool GameState::player_can_play(int player_id) const
{
    return player_can_play_.at(player_id);
}

std::ostream& operator<<(std::ostream& out, const GameState& gs)
{
    for (size_t i = 0; i < gs.get_board().size(); ++i)
    {
        if (i % 3 == 0 && i != 0)
            out << "\n";
        out << gs.get_board().at(i);
    }

    out << std::endl;

    return out;
}
