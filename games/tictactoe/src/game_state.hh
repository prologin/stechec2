// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2018 Association Prologin <association@prologin.org>
#pragma once

#include <rules/game-state.hh>
#include <rules/player.hh>

#include <unordered_map>
#include <vector>

#include "constant.hh"

class GameState : public rules::GameState
{
public:
    const int NO_PLAYER = -1;

    GameState(const rules::Players& players);
    GameState* copy() const override;

    std::vector<int> get_board() const;
    bool is_valid_cell(position pos) const;
    int get_cell(position pos) const;
    void set_cell(position pos, int player_id);

    int winner() const;
    bool is_board_full() const;
    bool is_finished() const;
    void compute_scores();

    void set_current_player(int player_id);
    int get_current_player() const;

    void set_player_can_play(int player_id, bool can_play);
    bool player_can_play(int player_id) const;

private:
    static constexpr size_t PLAYER_COUNT = 2;
    static constexpr int LAST_PLAYER = PLAYER_COUNT - 1;

    int current_player_;
    std::array<bool, PLAYER_COUNT> player_can_play_;
    std::vector<int> board_;
};

std::ostream& operator<<(std::ostream& out, const GameState& gs);
