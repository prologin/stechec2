// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#include "game_state.hh"

GameState::GameState(rules::Players_sptr players)
    : rules::GameState()
    , players_(players)
    , board_({NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER,
              NO_PLAYER, NO_PLAYER, NO_PLAYER})
{
    for (auto& player : players->players)
        if (player->type == rules::PLAYER)
            is_player_turn_.emplace(std::make_pair(player->id, false));
}

rules::GameState* GameState::copy() const
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

void GameState::set_player_turn(int player_id, bool state)
{
    assert(is_player_turn_.count(player_id) != 0);
    is_player_turn_.at(player_id) = state;
}

bool GameState::is_player_turn(int player_id) const
{
    assert(is_player_turn_.count(player_id) != 0);
    return is_player_turn_.at(player_id);
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
