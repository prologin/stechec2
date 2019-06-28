// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#pragma once

#include <string>
#include <vector>

#include <rules/types.hh>
#include <utils/buffer.hh>
#include <utils/dll.hh>

class Replay
{
public:
    bool check() const;
    void run();

private:
    utils::Buffer read_replay(const std::string& replay_path);
    std::string read_map(utils::Buffer* replay);
    rules::Players_sptr read_players(utils::Buffer* replay);
    rules::Players_sptr read_result(utils::Buffer* replay);

    bool compare_results(const rules::Players& ref,
                         const rules::Players& actual) const;
};
